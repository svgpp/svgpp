// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/detail/required_attributes_check.hpp>
#include <svgpp/parser/css_style_iterator.hpp>
#include <svgpp/detail/attribute_name_to_id.hpp>
#include <svgpp/detail/names_dictionary.hpp>
#include <svgpp/traits/attribute_groups.hpp>
#include <svgpp/attribute_traversal/common.hpp>
#include <svgpp/policy/error.hpp>
#include <svgpp/policy/xml/fwd.hpp>
#include <svgpp/template_parameters.hpp>
#include <bitset>
#include <boost/mpl/at.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/inherit.hpp>
#include <boost/mpl/empty_sequence.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/single_view.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/parameter.hpp>

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4127) // conditional expression is constant
#endif

namespace svgpp
{

template<class EventTag>
struct notify_context;

namespace traversal_detail
{
  struct priority_load_start
  {
    template<class Dispatcher, class AttributeLoadFunc>
    static bool execute(Dispatcher &, AttributeLoadFunc &)
    { return true; }
  };

  template<class Prev, class Attribute>
  struct priority_load_op
  {
    template<class Dispatcher, class AttributeLoadFunc>
    static bool execute(Dispatcher & dispatcher, AttributeLoadFunc & load_func)
    {
      if (!Prev::execute(dispatcher, load_func))
        return false;
      return load_func(Attribute::attribute_id);
    }
  };

  template<class Prev, class EventTag>
  struct priority_load_op<Prev, notify_context<EventTag> >
  {
    template<class Dispatcher, class AttributeLoadFunc>
    static bool execute(Dispatcher & dispatcher, AttributeLoadFunc & load_func)
    {
      if (!Prev::execute(dispatcher, load_func))
        return false;
      return dispatcher.notify(EventTag());
    }
  };

  template<class State, class AttributeTag>
  struct is_attribute_id_in_sequence_op
  {
    static bool check_attribute(detail::attribute_id id)
    {
      return AttributeTag::attribute_id == id || State::check_attribute(id);
    }
  };

  template<class State, class EventTag>
  struct is_attribute_id_in_sequence_op<State, notify_context<EventTag> >
  {
    static bool check_attribute(detail::attribute_id id)
    {
      return State::check_attribute(id);
    }
  };

  struct is_attribute_id_in_sequence_start
  {
    BOOST_CONSTEXPR static bool check_attribute(detail::attribute_id)
    {
      return false;
    }
  };

  template<class ValueSaver, bool ParseStyleAttribute>
  class found_attributes;
  
  template<class ValueSaver>
  class found_attributes<ValueSaver, true>
  {
  private:
    typename ValueSaver::attribute_or_css_saved_value attribute_or_css_saved_values_[detail::styling_attribute_count];
    typename ValueSaver::attribute_saved_value attribute_saved_values_[detail::attribute_count - detail::styling_attribute_count];
    std::bitset<detail::attribute_count> attribute_found_;
    std::bitset<detail::styling_attribute_count> css_found_;

  public:
    template<class XMLAttributesIterator>
    void save_attribute(detail::attribute_id id, XMLAttributesIterator const & xml_attributes_iterator)
    {
      attribute_found_.set(id);
      if (id <= detail::last_styling_attribute)
      {
        if (!css_found_.test(id))
          ValueSaver::save(xml_attributes_iterator, attribute_or_css_saved_values_
            [static_cast<size_t>(id)]);
      }
      else
        ValueSaver::save(xml_attributes_iterator, attribute_saved_values_
          [static_cast<size_t>(id - detail::styling_attribute_count)]);
    }

    template<class StyleValue>
    void save_css(detail::attribute_id style_id, StyleValue const & style_value)
    {
      ValueSaver::save(style_value, attribute_or_css_saved_values_[static_cast<size_t>(style_id)]);
      css_found_.set(static_cast<size_t>(style_id));
    }

    template<class Dispatcher, bool ClearFoundMark>
    class load_func: boost::noncopyable
    {
    public:
      load_func(Dispatcher & dispatcher, found_attributes<ValueSaver, true> & found_attributes)
        : found_attributes_(found_attributes)
        , dispatcher_(dispatcher)
      {
      }

      bool operator()(detail::attribute_id id) const
      {
        if (id < detail::styling_attribute_count)
        {
          if (found_attributes_.css_found_.test(id))
          {
            if (ClearFoundMark)
            {
              found_attributes_.css_found_.reset(id);
              found_attributes_.attribute_found_.reset(id);
            }

            return dispatcher_.load_attribute(id, 
              ValueSaver::get_css_value(found_attributes_.attribute_or_css_saved_values_[id]),
              tag::source::css());
          }
          else if (found_attributes_.attribute_found_.test(id))
          {
            if (ClearFoundMark)
              found_attributes_.attribute_found_.reset(id);

            typename ValueSaver::attribute_value value = ValueSaver::get_value(found_attributes_.attribute_or_css_saved_values_[id]);
            return dispatcher_.load_attribute(id, ValueSaver::get_string_range(value),
              tag::source::attribute());
          }
        }
        else if (found_attributes_.attribute_found_.test(id))
        {
          if (ClearFoundMark)
            found_attributes_.attribute_found_.reset(id);

          typename ValueSaver::attribute_value value = ValueSaver::get_value(
            found_attributes_.attribute_saved_values_[id - detail::styling_attribute_count]); 
          return dispatcher_.load_attribute(id, ValueSaver::get_string_range(value),
            tag::source::attribute());
        }
        return true;
      }

    private:
      found_attributes<ValueSaver, true> & found_attributes_;
      Dispatcher & dispatcher_;
    };
  };

  template<class ValueSaver>
  class found_attributes<ValueSaver, false>
  {
  private:
    typename ValueSaver::attribute_saved_value attribute_saved_values_[detail::attribute_count];
    std::bitset<detail::attribute_count> attribute_found_;

  public:
    template<class XMLAttributesIterator>
    void save_attribute(detail::attribute_id id, XMLAttributesIterator const & xml_attributes_iterator)
    {
      attribute_found_.set(id);
      ValueSaver::save(xml_attributes_iterator, attribute_saved_values_
        [static_cast<size_t>(id)]);
    }

    template<class Dispatcher, bool ClearFoundMark>
    class load_func: boost::noncopyable
    {
    public:
      load_func(Dispatcher & dispatcher, found_attributes<ValueSaver, false> & found_attributes)
        : found_attributes_(found_attributes)
        , dispatcher_(dispatcher)
      {
      }

      bool operator()(detail::attribute_id id) const
      {
        if (found_attributes_.attribute_found_.test(id))
        {
          if (ClearFoundMark)
            found_attributes_.attribute_found_.reset(id);

          typename ValueSaver::attribute_value value = ValueSaver::get_value(
            found_attributes_.attribute_saved_values_[id]); 
          return dispatcher_.load_attribute(id, ValueSaver::get_string_range(value),
            tag::source::attribute());
        }
        else
          return true;
      }

    private:
      found_attributes<ValueSaver, false> & found_attributes_;
      Dispatcher & dispatcher_;
    };
  };

  template<class XMLAttributesIterator, class XMLPolicy, class Enable = void>
  struct attribute_value_saver;

  template<class XMLAttributesIterator, class XMLPolicy>
  struct attribute_value_saver<XMLAttributesIterator, XMLPolicy,
    typename boost::enable_if<boost::is_same<typename XMLPolicy::saved_value_type, typename XMLPolicy::string_type> >::type>
  {
    typedef typename XMLPolicy::string_type attribute_value;
    typedef typename XMLPolicy::string_type attribute_saved_value;
    typedef typename XMLPolicy::string_type attribute_or_css_saved_value;

    // Store attribute value
    static void save(XMLAttributesIterator const & xml_attributes_iterator,
      attribute_saved_value & store)
    {
      store = XMLPolicy::save_value(xml_attributes_iterator);
    }

    // Store 'style' attribute substring
    static void save(typename XMLPolicy::string_type const & range,
      attribute_or_css_saved_value & store)
    {
      store = range;
    }

    static attribute_value get_value(attribute_saved_value const & store)
    {
      return store;
    }

    static typename XMLPolicy::string_type get_string_range(attribute_value const & value)
    {
      return value;
    }

    static typename XMLPolicy::string_type const & get_css_value(attribute_or_css_saved_value const & store)
    {
      return store;
    }
  };

  template<class XMLAttributesIterator, class XMLPolicy>
  struct attribute_value_saver<XMLAttributesIterator, XMLPolicy,
    typename boost::disable_if<boost::is_same<typename XMLPolicy::saved_value_type, typename XMLPolicy::string_type> >::type>
  {
    typedef typename XMLPolicy::attribute_value_type attribute_value;
    typedef typename XMLPolicy::saved_value_type attribute_saved_value;
    typedef std::pair<typename XMLPolicy::saved_value_type, typename XMLPolicy::string_type> attribute_or_css_saved_value;

    // Store attribute value
    static void save(XMLAttributesIterator const & xml_attributes_iterator,
      attribute_saved_value & store)
    {
      store = XMLPolicy::save_value(xml_attributes_iterator);
    }

    // Store attribute value
    static void save(XMLAttributesIterator const & xml_attributes_iterator,
      attribute_or_css_saved_value & store)
    {
      store.first = XMLPolicy::save_value(xml_attributes_iterator);
    }

    // Store 'style' attribute substring
    static void save(typename XMLPolicy::string_type const & range,
      attribute_or_css_saved_value & store)
    {
      store.second = range;
    }

    static attribute_value get_value(attribute_saved_value const & store)
    {
      return XMLPolicy::get_value(store);
    }

    static attribute_value get_value(attribute_or_css_saved_value const & store)
    {
      return XMLPolicy::get_value(store.first);
    }

    static typename XMLPolicy::string_type get_string_range(attribute_value const & value)
    {
      return XMLPolicy::get_string_range(value);
    }

    static typename XMLPolicy::string_type const & get_css_value(attribute_or_css_saved_value const & store)
    {
      return store.second;
    }
  };
} // namespace traversal_detail

template<class AttributeTraversalPolicy, SVGPP_TEMPLATE_ARGS_DEF>
struct attribute_traversal_prioritized
{
  typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::xml_attribute_policy>,
      boost::parameter::optional<tag::error_policy>,
      boost::parameter::optional<tag::css_name_to_id_policy>
  >::bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::css_name_to_id_policy, 
    policy::css_name_to_id::default_policy>::type css_name_to_id_policy;

  template<class XMLAttributesIterator, class Dispatcher>
  static bool load(XMLAttributesIterator xml_attributes_iterator, Dispatcher & dispatcher)
  {
    typedef typename boost::parameter::value_type<args, tag::xml_attribute_policy, 
      policy::xml::attribute_iterator<XMLAttributesIterator> >::type xml_policy;

    typedef traversal_detail::attribute_value_saver<XMLAttributesIterator, xml_policy> value_saver;
    typedef traversal_detail::found_attributes<value_saver, AttributeTraversalPolicy::parse_style> found_attributes;
    typedef typename boost::parameter::value_type<args, tag::error_policy, 
      policy::error::default_policy<typename Dispatcher::context_type> >::type error_policy_t;

    detail::required_attributes_check<typename AttributeTraversalPolicy::required_attributes> required_check;
    found_attributes found;
    typename xml_policy::attribute_value_type style_value; // Iterators in the value persist till the end of function
    for(; !xml_policy::is_end(xml_attributes_iterator); xml_policy::advance(xml_attributes_iterator))
    {
      BOOST_SCOPED_ENUM(detail::namespace_id) ns = xml_policy::get_namespace(xml_attributes_iterator);
      if (ns == detail::namespace_id::other)
        continue;
      typename xml_policy::attribute_name_type attribute_name = xml_policy::get_local_name(xml_attributes_iterator);
      detail::attribute_id id = detail::attribute_name_to_id(ns, xml_policy::get_string_range(attribute_name));
      switch (id)
      {
      case detail::unknown_attribute_id:
        if (!error_policy_t::unknown_attribute(dispatcher.context(), 
          xml_policy::get_attribute(xml_attributes_iterator), 
          xml_policy::get_string_range(attribute_name), ns, tag::source::attribute()))
          return false;
        break;
      case detail::attribute_id_style:
        if (AttributeTraversalPolicy::parse_style)
        {
          if (!load_style<xml_policy, error_policy_t>(xml_attributes_iterator, dispatcher, style_value, found))
            return false;
          break;
        }
      default:
        required_check(id);
        found.save_attribute(id, xml_attributes_iterator);
        break;
      }
    }

    detail::missing_attribute_visitor<error_policy_t> visitor(dispatcher.context());
    if (!required_check.visit_missing(visitor))
      return false;
    
    {
      // Load priority attributes in required order,
      // mark them as loaded
      typedef typename boost::mpl::fold<
        typename AttributeTraversalPolicy::priority_attributes, 
        traversal_detail::priority_load_start,
        traversal_detail::priority_load_op<boost::mpl::_1, boost::mpl::_2> 
      >::type priority_load_operations_t;

      typename found_attributes::template load_func<Dispatcher, true> load_func(dispatcher, found);
      if (!priority_load_operations_t::execute(dispatcher, load_func))
        return false;
    }

    typedef typename boost::mpl::fold<
      typename AttributeTraversalPolicy::deferred_attributes,
      traversal_detail::is_attribute_id_in_sequence_start,
      traversal_detail::is_attribute_id_in_sequence_op<boost::mpl::_1, boost::mpl::_2>
    >::type is_deferred_t;

    // Load remaining attributes, excluding deferred
    typedef typename found_attributes::template load_func<Dispatcher, false> load_func_t;
    load_func_t load_func(dispatcher, found);
    for(size_t id = 0; id < detail::attribute_count; ++id)
      if (!is_deferred_t::check_attribute(static_cast<detail::attribute_id>(id)))
        if (!load_func(static_cast<detail::attribute_id>(id)))
          return false;

    // Loading deferred attributes
    typedef typename boost::mpl::fold<
      typename AttributeTraversalPolicy::deferred_attributes, 
      traversal_detail::priority_load_start,
      traversal_detail::priority_load_op<boost::mpl::_1, boost::mpl::_2> 
    >::type deferred_load_operations_t;

    return deferred_load_operations_t::execute(dispatcher, load_func);
  }

private:
  template<class XMLPolicy, class ErrorPolicy, class XMLAttributesIterator, class Dispatcher, class FoundAttributes>
  static bool load_style(XMLAttributesIterator const & xml_attributes_iterator, Dispatcher & dispatcher,
    typename XMLPolicy::attribute_value_type & style_value,
    FoundAttributes & found,
    typename boost::enable_if_c<AttributeTraversalPolicy::parse_style && (true || boost::is_void<XMLAttributesIterator>::value)>::type * = NULL)
  {
    style_value = XMLPolicy::get_value(xml_attributes_iterator);
    typename XMLPolicy::string_type style_string = XMLPolicy::get_string_range(style_value);
    typedef css_style_iterator<typename boost::range_iterator<typename XMLPolicy::string_type>::type> css_iterator;
    for(css_iterator it(boost::begin(style_string), boost::end(style_string)); !it.eof(); ++it)
    {
      detail::attribute_id style_id = css_name_to_id_policy::find(it->first);
      if (style_id == detail::unknown_attribute_id)
      {
        if (!ErrorPolicy::unknown_attribute(dispatcher.context(), 
          XMLPolicy::get_attribute(xml_attributes_iterator), 
          it->first, tag::source::css()))
          return false;
      }
      else
      {
        found.save_css(style_id, it->second);
      }
    }
    return true;
  }

  template<class XMLPolicy, class ErrorPolicy, class XMLAttributesIterator, class Dispatcher, class FoundAttributes>
  static bool load_style(XMLAttributesIterator const &, Dispatcher &,
    typename XMLPolicy::attribute_value_type &,
    FoundAttributes &,
    typename boost::disable_if_c<AttributeTraversalPolicy::parse_style && (true || boost::is_void<XMLAttributesIterator>::value)>::type * = NULL)
  {
    BOOST_ASSERT(false); // Must not be called
    return true;
  }
};

}

#ifdef _MSC_VER
# pragma warning(pop)
#endif
