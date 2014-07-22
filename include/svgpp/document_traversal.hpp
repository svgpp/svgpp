// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/attribute_traversal.hpp>
#include <svgpp/attribute_dispatcher.hpp>
#include <svgpp/template_parameters.hpp>
#include <svgpp/detail/element_id_to_tag.hpp>
#include <svgpp/policy/document_traversal_control.hpp>
#include <svgpp/policy/load_text.hpp>
#include <svgpp/traits/child_element_types.hpp>
#include <svgpp/traits/element_with_text_content.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/parameter.hpp>
#include <boost/static_assert.hpp>
#include <boost/pointee.hpp>
#include <boost/preprocessor.hpp>
#include <boost/type_traits.hpp>

namespace svgpp
{

namespace context_factory
{

template<class ParentContext, class ElementTag>
class same: boost::noncopyable
{
public:
  typedef ParentContext type;

  template<class XMLElement>
  same(ParentContext & context, XMLElement const &)
    : context_(context)
  {
    context_.on_enter_element(ElementTag());
  }

  type & get() const { return context_; }

  void on_exit_element() const
  {
    context_.on_exit_element();
  }

private:
  type & context_;
};

template<class ParentContext, class ChildContext>
class on_stack: boost::noncopyable
{
public:
  typedef ChildContext type;

  template<class XMLElement>
  on_stack(ParentContext & context, XMLElement const &)
    : context_(context)
  {
  }

  type & get() { return context_; }

  void on_exit_element() 
  {
    context_.on_exit_element();
  }

private:
  type context_;
};

template<
  class ParentContext, 
  class ElementTag, 
  class ChildContextPtr, 
  class ChildContext = typename boost::pointee<ChildContextPtr>::type 
>
class get_ptr_from_parent: boost::noncopyable
{
public:
  typedef ChildContext type;

  template<class XMLElement, class LoaderState>
  get_ptr_from_parent(ParentContext & context, XMLElement const &)
    : context_(context.get_child_context(ElementTag()))
  {
  }

  type & get() const { return *context_; }

  void on_exit_element() const
  {
    context_->on_exit_element();
  }

private:
  ChildContextPtr context_;
};

} // namespace context_factory

struct default_context_factories
{
  template<class ParentContext, class ElementTag>
  struct apply
  {
    typedef context_factory::same<ParentContext, ElementTag> type;
  };
};

BOOST_PARAMETER_TEMPLATE_KEYWORD(context_factories)
BOOST_PARAMETER_TEMPLATE_KEYWORD(expected_elements)
BOOST_PARAMETER_TEMPLATE_KEYWORD(xml_element_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(load_text_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(ignored_elements)
BOOST_PARAMETER_TEMPLATE_KEYWORD(processed_elements)
BOOST_PARAMETER_TEMPLATE_KEYWORD(document_traversal_control_policy)

template<SVGPP_TEMPLATE_ARGS2_DEF>
class document_traversal
{
protected:
  typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::context_factories>
    , boost::parameter::optional<tag::xml_element_policy>
    , boost::parameter::optional<tag::load_text_policy>
    , boost::parameter::optional<tag::error_policy>
    , boost::parameter::optional<tag::ignored_elements, boost::mpl::is_sequence<boost::mpl::_> >
    , boost::parameter::optional<tag::processed_elements, boost::mpl::is_sequence<boost::mpl::_> >
    , boost::parameter::optional<tag::document_traversal_control_policy>
  >::template bind<SVGPP_TEMPLATE_ARGS2_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::context_factories, 
    default_context_factories>::type context_factories;
  typedef typename boost::parameter::value_type<args, tag::ignored_elements, 
    boost::mpl::empty_sequence>::type ignored_elements;
  typedef typename boost::parameter::value_type<args, tag::processed_elements, 
    boost::mpl::empty_sequence>::type processed_elements;
  typedef typename boost::parameter::value_type<args, tag::document_traversal_control_policy, 
    policy::document_traversal_control::stub>::type traversal_control_policy;

  BOOST_STATIC_ASSERT_MSG(boost::mpl::empty<ignored_elements>::value 
    || boost::mpl::empty<processed_elements>::value, "Only one of ignored_elements and processed_elements may be non-empty");

  typedef typename
    boost::mpl::if_<
      boost::mpl::not_<boost::mpl::empty<processed_elements> >,
      boost::mpl::has_key<boost::mpl::protect<processed_elements>, boost::mpl::_1>,
      boost::mpl::not_<boost::mpl::has_key<boost::mpl::protect<ignored_elements>, boost::mpl::_1> >  
    >::type is_element_processed;

public:
  template<class XMLElement, class Context>
  static bool load_document(XMLElement & xml_element_svg, Context & context)
  {
    return load_element<traits::child_element_types<tag::element::svg>::type, void>
      (xml_element_svg, context, tag::element::svg());
  }

  template<class ExpectedChildElements, class ReferencingElement, class XMLElement, class Context, class ElementTag>
  static bool load_element(XMLElement & xml_element, Context & context, ElementTag element_tag)
  {
    return load_attributes<ReferencingElement>(xml_element, context, element_tag)
      && (!traversal_control_policy::proceed_to_element_content(context)
        || load_element_content<ExpectedChildElements>(xml_element, context, element_tag));
  }

  template<class ReferencingElement, class XMLElement, class Context, class ElementTag>
  static bool load_attributes(XMLElement & xml_element, Context & context, ElementTag element_tag)
  {
    typedef typename boost::parameter::value_type<args, tag::xml_element_policy, 
      policy::xml::element_iterator<XMLElement> >::type xml_policy_t;

    typedef typename dispatcher_detail::referencing_element_if_needed<
      ReferencingElement,
      ElementTag,
      Context,
      SVGPP_TEMPLATE_ARGS2_PASS
    >::type referencing_element_tag;
    typedef attribute_dispatcher<
      ElementTag, 
      Context, 
      referencing_element<referencing_element_tag>,
      SVGPP_TEMPLATE_ARGS2_PASS
    > attribute_dispatcher_t;
    static const bool parse_style_attribute = 
      !boost::mpl::has_key<typename attribute_dispatcher_t::passthrough_attributes, tag::attribute::style>::value
      && boost::mpl::apply<typename attribute_dispatcher_t::is_attribute_processed, tag::attribute::style>::type::value;
    attribute_dispatcher_t attribute_dispatcher(context);
    if (!attribute_traversal<
        ElementTag,
        parse_style_attribute,
        SVGPP_TEMPLATE_ARGS2_PASS
      >::type::load(xml_policy_t::get_attributes(xml_element), attribute_dispatcher))
      return false;
    return attribute_dispatcher.on_exit_attributes();
  }

  template<class ExpectedChildElements, class ReferencingElement, class XMLElement, class ParentContext, class ElementTag>
  static bool load_child_element(XMLElement & xml_element, ParentContext & parent_context, ElementTag element_tag)
  {
    typedef typename context_factories::template apply<ParentContext, ElementTag>::type context_factory_t;
    context_factory_t context_factory(parent_context, xml_element);
    if (!load_element<ExpectedChildElements, ReferencingElement>(xml_element, context_factory.get(), element_tag))
      return false;
    context_factory.on_exit_element();
    return true;
  }

  // TODO: Only if child elements or text nodes supported
  template<class ExpectedChildElements, class XMLElement, class Context, class ElementTag>
  static typename boost::enable_if<
    boost::mpl::and_<
      boost::mpl::empty<typename traits::child_element_types<ElementTag>::type>,
      boost::mpl::not_<traits::element_with_text_content<ElementTag> > >, bool>::type
  load_element_content(XMLElement const &, Context const &, ElementTag)
  {
    return true;
  }

  template<class ExpectedChildElements, class XMLElement, class Context, class ElementTag>
  static typename boost::disable_if<
    boost::mpl::or_<
      boost::mpl::empty<typename traits::child_element_types<ElementTag>::type>,
      traits::element_with_text_content<ElementTag> >, bool>::type
  load_element_content(XMLElement & xml_element, Context & context, ElementTag element_tag)
  {
    typedef typename boost::parameter::value_type<args, tag::xml_element_policy, 
      policy::xml::element_iterator<XMLElement> >::type xml_policy_t;
    typedef typename boost::parameter::value_type<args, tag::error_policy, 
      policy::error::default_policy<Context> >::type error_policy;

    for(typename xml_policy_t::iterator_type xml_child_element = xml_policy_t::get_child_elements(xml_element); 
      !xml_policy_t::is_end(xml_child_element); xml_policy_t::advance_element(xml_child_element))
    {
      if (!load_child_xml_element<ExpectedChildElements, is_element_processed, void>(
          xml_child_element, context, element_tag))
        return false;
      if (!traversal_control_policy::proceed_to_next_child(context))
        break;
    }
    return true;
  }

  template<class ExpectedChildElements, class XMLElement, class Context, class ElementTag>
  static typename boost::enable_if<traits::element_with_text_content<ElementTag>, bool>::type
  load_element_content(XMLElement & xml_element, Context & context, ElementTag element_tag)
  {
    typedef typename boost::parameter::value_type<args, tag::xml_element_policy, 
      policy::xml::element_iterator<XMLElement> >::type xml_policy_t;
    typedef typename boost::parameter::value_type<args, tag::load_text_policy, 
      policy::load_text::default_policy<Context> >::type load_text_policy;
    
    for(typename xml_policy_t::iterator_type xml_child_element = xml_policy_t::get_child_elements_and_texts(xml_element); 
      !xml_policy_t::is_end(xml_child_element); xml_policy_t::advance_element_or_text(xml_child_element))
    {
      if (xml_policy_t::is_text(xml_child_element))
        load_text_policy::set_text(context, xml_policy_t::get_text(xml_child_element));
      else
      {
        if (!load_child_xml_element<ExpectedChildElements, is_element_processed, false>(
            xml_child_element, context, element_tag))
          return false;
      }
      if (!traversal_control_policy::proceed_to_next_child(context))
        break;
    }
    return true;
  }

  template<SVGPP_TEMPLATE_ARGS_DEF>
  struct load_referenced_element
  {
    template<class XMLElement, class Context>
    static bool load(XMLElement & xml_element, Context & parent_context)
    {
      typedef typename boost::parameter::parameters<
          boost::parameter::optional<tag::referencing_element>
        , boost::parameter::required<tag::expected_elements, boost::mpl::is_sequence<boost::mpl::_> >
        , boost::parameter::optional<tag::ignored_elements, boost::mpl::is_sequence<boost::mpl::_> >
        , boost::parameter::optional<tag::processed_elements, boost::mpl::is_sequence<boost::mpl::_> >
      >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
      typedef typename boost::parameter::value_type<args, tag::referencing_element, void>::type referencing_element;
      typedef typename boost::parameter::value_type<args, tag::expected_elements>::type expected_elements;
      typedef typename boost::parameter::value_type<args, tag::ignored_elements, void>::type ignored_elements;
      typedef typename boost::parameter::value_type<args, tag::processed_elements, void>::type processed_elements;

      BOOST_STATIC_ASSERT_MSG(boost::is_void<ignored_elements>::value 
        || boost::is_void<processed_elements>::value, "Only one of ignored_elements and processed_elements may be set");

      // Elements other than ExpectedElements will cause unexpected_element error

      // If neither ignored_elements nor processed_elements are set then expected_elements treated as processed_elements
      typedef typename
        boost::mpl::if_<
          boost::mpl::not_<boost::is_void<processed_elements> >,
          boost::mpl::has_key<boost::mpl::protect<processed_elements>, boost::mpl::_1>,
          typename boost::mpl::if_<
            boost::mpl::not_<boost::is_void<ignored_elements> >,
            boost::mpl::not_<boost::mpl::has_key<boost::mpl::protect<ignored_elements>, boost::mpl::_1> >,
            boost::mpl::has_key<boost::mpl::protect<expected_elements>, boost::mpl::_1>
          >::type
        >::type is_element_processed;

      // The 'a' element may contain any element that its parent may contain, except itself.
      // So we can't handle 'a' without knowing its parent.
      BOOST_MPL_ASSERT_NOT((boost::mpl::has_key<expected_elements, tag::element::a>));
      // TODO: Pass ReferencingElement only when needed, to reduce number of instantiations
      return load_child_xml_element<
          expected_elements, 
          is_element_processed, 
          referencing_element
        >(xml_element, parent_context, 
          boost::mpl::void_()); // ParentElementTag parameter can be of any type, it shouldn't be used for
                                // any element except 'a'
    }
  };

protected:
  template<
    class XMLElement, 
    class Context, 
    class IsElementProcessed,
    class ParentElementTag, 
    class ReferencingElement
  >
  class load_element_functor: boost::noncopyable
  {
  public:
    load_element_functor(XMLElement & xml_element, Context & context)
      : xml_element_(xml_element)
      , context_(context)
      , result_(true)
    {
    }

    template<class ElementTag>
    typename boost::enable_if_c<
      !boost::is_same<ElementTag, tag::element::a>::value
      && boost::mpl::apply<IsElementProcessed, ElementTag>::type::value
    >::type operator()(ElementTag tag) 
    {
      result_ = document_traversal::load_child_element<
        typename traits::child_element_types<ElementTag>::type, 
        ReferencingElement
      >(xml_element_, context_, tag);
    }

    template<class ElementTag>
    typename boost::enable_if_c<
      boost::is_same<ElementTag, tag::element::a>::value
      && boost::mpl::apply<IsElementProcessed, ElementTag>::type::value
    >::type operator()(ElementTag tag) 
    {
      // The 'a' element may contain any element that its parent may contain, except itself.
      // Removing 'a' itself from list of permitted child types
      result_ = document_traversal::load_child_element<
        typename boost::mpl::erase_key<
          typename traits::child_element_types<ParentElementTag>::type,
          tag::element::a
        >::type,
        ReferencingElement
      >(xml_element_, context_, tag);
    }

    template<class ElementTag>
    typename boost::disable_if<typename boost::mpl::apply<IsElementProcessed, ElementTag>::type>::type
    operator()(ElementTag) const
    {}

    bool succeeded() const 
    { 
      return result_; 
    }

  private:
    XMLElement & xml_element_;
    Context & context_;
    bool result_;
  };

  template<
    class ExpectedElements, 
    class IsElementProcessed,
    class ReferencingElement, 
    class XMLElement, 
    class Context, 
    class ParentElementTag
  >
  static bool load_child_xml_element(XMLElement & xml_element, Context & parent_context, ParentElementTag)
  {
    typedef typename boost::parameter::value_type<args, tag::xml_element_policy, 
      policy::xml::element_iterator<XMLElement> >::type xml_policy_t;
    typedef typename boost::parameter::value_type<args, tag::error_policy, 
      policy::error::default_policy<Context> >::type error_policy;

    typename xml_policy_t::element_name_type element_name = xml_policy_t::get_local_name(xml_element);
    detail::element_type_id element_type_id = detail::element_name_to_id_dictionary::find(
      xml_policy_t::get_string_range(element_name));
    if (element_type_id != detail::unknown_element_type_id)
    {
      load_element_functor<XMLElement, Context, IsElementProcessed, ParentElementTag, ReferencingElement> load_functor
        (xml_element, parent_context);
      if (detail::id_to_element_tag<ExpectedElements>(element_type_id, load_functor))
        return load_functor.succeeded();
      else
        return error_policy::unexpected_element(parent_context, xml_element);
    }
    else
      return error_policy::unknown_element(parent_context, xml_element, xml_policy_t::get_string_range(element_name));
  }
};

}