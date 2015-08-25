// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/template_parameters.hpp>
#include <svgpp/attribute_traversal/common.hpp>
#include <svgpp/detail/attribute_name_to_id.hpp>
#include <svgpp/detail/required_attributes_check.hpp>
#include <svgpp/parser/css_style_iterator.hpp>
#include <svgpp/policy/error.hpp>
#include <svgpp/policy/xml/fwd.hpp>
#include <svgpp/detail/names_dictionary.hpp>
#include <boost/mpl/if.hpp>

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4127) // conditional expression is constant
#endif

namespace svgpp
{

template<class RequiredAttributes, bool ParseStyleAttribute = true, SVGPP_TEMPLATE_ARGS_DEF>
class attribute_traversal_sequential
{
private:
  typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::xml_attribute_policy>,
      boost::parameter::optional<tag::error_policy>,
      boost::parameter::optional<tag::css_name_to_id_policy>
  >::bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::css_name_to_id_policy, 
    policy::css_name_to_id::default_policy>::type css_name_to_id_policy;

public:
  template<class XMLAttributesIterator, class Dispatcher>
  static bool load(XMLAttributesIterator xml_attributes_iterator, Dispatcher & dispatcher)
  {
    typedef typename boost::parameter::value_type<args, tag::xml_attribute_policy, 
      policy::xml::attribute_iterator<XMLAttributesIterator> >::type xml_policy;

    typedef typename boost::parameter::value_type<args, tag::error_policy, 
      policy::error::default_policy<typename Dispatcher::context_type> >::type error_policy;

    detail::required_attributes_check<RequiredAttributes> required_check;
    for(; !xml_policy::is_end(xml_attributes_iterator); 
      xml_policy::advance(xml_attributes_iterator))
    {
      BOOST_SCOPED_ENUM(detail::namespace_id) ns = xml_policy::get_namespace(xml_attributes_iterator);
      if (ns == detail::namespace_id::other)
        continue;
      typename xml_policy::attribute_name_type attribute_name = xml_policy::get_local_name(xml_attributes_iterator);
      detail::attribute_id id = detail::attribute_name_to_id(ns, xml_policy::get_string_range(attribute_name));
      switch (id)
      {
      case detail::unknown_attribute_id:
        if (!error_policy::unknown_attribute(dispatcher.context(), 
          xml_policy::get_attribute(xml_attributes_iterator), 
          xml_policy::get_string_range(attribute_name), ns, tag::source::attribute()))
          return false;
        break;
      case detail::attribute_id_style:
      {
        if (ParseStyleAttribute)
        {
          if (!load_style<xml_policy, error_policy>(xml_attributes_iterator, dispatcher))
            return false;
          break;
        }
      }
      default:
      {
        typename xml_policy::attribute_value_type value = xml_policy::get_value(xml_attributes_iterator);
        if (!dispatcher.load_attribute(id, xml_policy::get_string_range(value), tag::source::attribute()))
          return false;
        required_check(id);
      }
      }
    }

    detail::missing_attribute_visitor<error_policy> visitor(dispatcher.context());
    return required_check.visit_missing(visitor);
  }

private:
  template<class XMLPolicy, class ErrorPolicy, class XMLAttributesIterator, class Dispatcher>
  static bool load_style(XMLAttributesIterator const & xml_attributes_iterator, Dispatcher & dispatcher,
    typename boost::enable_if_c<ParseStyleAttribute && (true || boost::is_void<XMLAttributesIterator>::value)>::type * = NULL)
  {
    typename XMLPolicy::attribute_value_type style_value = XMLPolicy::get_value(xml_attributes_iterator);
    typename XMLPolicy::string_type style_string = XMLPolicy::get_string_range(style_value);
    typedef css_style_iterator<typename boost::range_iterator<typename XMLPolicy::string_type>::type> css_iterator;
    for(css_iterator it(boost::begin(style_string), boost::end(style_string)); !it.eof(); ++it)
    {
      detail::attribute_id style_id = css_name_to_id_policy::find(it->first);
      if (style_id == detail::unknown_attribute_id)
      {
        if (!ErrorPolicy::unknown_attribute(dispatcher.context(), 
          XMLPolicy::get_attribute(xml_attributes_iterator), it->first, tag::source::css()))
          return false;
      }
      else
        if (!dispatcher.load_attribute(style_id, it->second, tag::source::css()))
          return false;
    }
    return true;
  }

  template<class XMLPolicy, class ErrorPolicy, class XMLAttributesIterator, class Dispatcher>
  static bool load_style(XMLAttributesIterator const &, Dispatcher &,
    typename boost::disable_if_c<ParseStyleAttribute && (true || boost::is_void<XMLAttributesIterator>::value)>::type * = NULL)
  { 
    BOOST_ASSERT(false); // Must not be called
    return true; 
  }
};

}

#ifdef _MSC_VER
# pragma warning(pop)
#endif
