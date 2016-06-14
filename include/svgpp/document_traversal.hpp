// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/attribute_traversal/attribute_traversal.hpp>
#include <svgpp/attribute_dispatcher.hpp>
#include <svgpp/config.hpp>
#include <svgpp/factory/context.hpp>
#include <svgpp/template_parameters.hpp>
#include <svgpp/detail/element_id_to_tag.hpp>
#include <svgpp/policy/document_traversal_control.hpp>
#include <svgpp/policy/text_events.hpp>
#include <svgpp/traits/child_element_types.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/parameter.hpp>
#include <boost/static_assert.hpp>
#include <boost/preprocessor.hpp>
#include <boost/tti/member_type.hpp>
#include <boost/type_traits.hpp>

namespace svgpp
{

struct default_context_factories
{
  template<class ParentContext, class ElementTag>
  struct apply
  {
    typedef factory::context::same<ParentContext, ElementTag> type;
  };
};

namespace detail
{
  BOOST_TTI_MEMBER_TYPE(intercepted_exception_type)

  struct dummy_exception {};

  template<class ErrorPolicy, class ExceptionType, class XMLElement>
  inline bool call_add_element_info(ExceptionType & e, XMLElement const & element)
  {
    return ErrorPolicy::add_element_info(e, element);
  }

  template<class ErrorPolicy, class XMLElement>
  inline bool call_add_element_info(dummy_exception & e, XMLElement const &)
  {
    return true;
  }
}

BOOST_PARAMETER_TEMPLATE_KEYWORD(context_factories)
BOOST_PARAMETER_TEMPLATE_KEYWORD(expected_elements)
BOOST_PARAMETER_TEMPLATE_KEYWORD(xml_element_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(text_events_policy)
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
    , boost::parameter::optional<tag::text_events_policy>
    , boost::parameter::optional<tag::error_policy>
    , boost::parameter::optional<tag::ignored_elements, boost::mpl::is_sequence<boost::mpl::_> >
    , boost::parameter::optional<tag::processed_elements, boost::mpl::is_sequence<boost::mpl::_> >
    , boost::parameter::optional<tag::document_traversal_control_policy>
  >::template bind<SVGPP_TEMPLATE_ARGS2_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::context_factories, 
    default_context_factories>::type context_factories;
  typedef typename boost::parameter::value_type<args, tag::ignored_elements, void>::type ignored_elements;
  typedef typename boost::parameter::value_type<args, tag::processed_elements, void>::type processed_elements;

  BOOST_STATIC_ASSERT_MSG(boost::is_void<ignored_elements>::value 
    != boost::is_void<processed_elements>::value, "Only one of ignored_elements and processed_elements must be set");

  typedef typename
    boost::mpl::if_<
      boost::is_void<ignored_elements>,
      boost::mpl::has_key<boost::mpl::protect<processed_elements>, boost::mpl::_1>,
      boost::mpl::not_<boost::mpl::has_key<boost::mpl::protect<ignored_elements>, boost::mpl::_1> >  
    >::type is_element_processed;

public:
  template<class XMLElement, class Context>
  static bool load_document(XMLElement const & xml_element_svg, Context & context)
  {
    return load_expected_element(xml_element_svg, context, tag::element::svg());
  }

  template<class XMLElement, class Context, class ElementTag>
  static bool load_expected_element(XMLElement const & xml_element, Context & context, ElementTag expected_element)
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
      if (element_type_id == ElementTag::element_id)
        return 
          load_element<
            typename traits::child_element_types<ElementTag>::type, 
            void
          >(xml_element, context, expected_element);
      else
        return error_policy::unexpected_element(context, xml_element);
    }
    else
      return error_policy::unknown_element(context, xml_element, xml_policy_t::get_string_range(element_name));
  }

  template<class ExpectedChildElements, class ReferencingElement, class XMLElement, class Context, class ElementTag>
  static bool load_element(XMLElement const & xml_element, Context & context, ElementTag element_tag)
  {
#ifdef SVGPP_INTERCEPT_EXCEPTIONS
    typedef typename boost::parameter::value_type<args, tag::error_policy, 
      policy::error::default_policy<Context> >::type error_policy;
    try
#endif
    {
      typedef typename boost::parameter::value_type<args, tag::document_traversal_control_policy, 
        policy::document_traversal_control::default_policy<Context> >::type traversal_control_policy;

      return load_attributes<ReferencingElement>(xml_element, context, element_tag)
        && (!traversal_control_policy::proceed_to_element_content(context)
          || load_element_content<ExpectedChildElements>(xml_element, context, element_tag));
    }
#ifdef SVGPP_INTERCEPT_EXCEPTIONS
    catch(typename detail::member_type_intercepted_exception_type<error_policy, detail::dummy_exception>::type & e)
    {
      return detail::call_add_element_info<error_policy>(e, xml_element);
    }
#endif
  }

  template<class ReferencingElement, class XMLElement, class Context, class ElementTag>
  static bool load_attributes(XMLElement const & xml_element, Context & context, ElementTag)
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
    attribute_dispatcher_t attribute_dispatcher(context);
    if (!attribute_traversal<
        ElementTag,
        SVGPP_TEMPLATE_ARGS2_PASS
      >::type::load(xml_policy_t::get_attributes(xml_element), attribute_dispatcher))
      return false;
    return attribute_dispatcher.on_exit_attributes();
  }

  template<class ExpectedChildElements, class ReferencingElement, class XMLElement, class ParentContext, class ElementTag>
  static bool load_child_element(XMLElement const & xml_element, ParentContext & parent_context, ElementTag element_tag)
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
      boost::mpl::not_<boost::mpl::has_key<ExpectedChildElements, tag::text_content> > >, bool>::type
  load_element_content(XMLElement const &, Context const &, ElementTag)
  {
    return true;
  }

  template<class ExpectedChildElements, class XMLElement, class Context, class ElementTag>
  static typename boost::disable_if<
    boost::mpl::or_<
      boost::mpl::empty<typename traits::child_element_types<ElementTag>::type>,
      boost::mpl::has_key<ExpectedChildElements, tag::text_content> >, bool>::type
  load_element_content(XMLElement const & xml_element, Context & context, ElementTag element_tag)
  {
    typedef typename boost::parameter::value_type<args, tag::xml_element_policy, 
      policy::xml::element_iterator<XMLElement> >::type xml_policy_t;
    typedef typename boost::parameter::value_type<args, tag::error_policy, 
      policy::error::default_policy<Context> >::type error_policy;
    typedef typename boost::parameter::value_type<args, tag::document_traversal_control_policy, 
      policy::document_traversal_control::default_policy<Context> >::type traversal_control_policy;

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
  static typename boost::enable_if<boost::mpl::has_key<ExpectedChildElements, tag::text_content>, bool>::type
  load_element_content(XMLElement const & xml_element, Context & context, ElementTag element_tag)
  {
    typedef typename boost::parameter::value_type<args, tag::xml_element_policy, 
      policy::xml::element_iterator<XMLElement> >::type xml_policy_t;
    typedef typename boost::parameter::value_type<args, tag::text_events_policy, 
      policy::text_events::default_policy<Context> >::type text_events_policy;
    typedef typename boost::parameter::value_type<args, tag::document_traversal_control_policy, 
      policy::document_traversal_control::default_policy<Context> >::type traversal_control_policy;
    
    for(typename xml_policy_t::iterator_type xml_child_element = xml_policy_t::get_child_elements_and_texts(xml_element); 
      !xml_policy_t::is_end(xml_child_element); xml_policy_t::advance_element_or_text(xml_child_element))
    {
      if (xml_policy_t::is_text(xml_child_element))
      {
        typename xml_policy_t::element_text_type text = xml_policy_t::get_text(xml_child_element);
        text_events_policy::set_text(context, xml_policy_t::get_string_range(text));
      }
      else
      {
        if (traversal_control_policy::process_child(context, xml_child_element))
          if (!load_child_xml_element<ExpectedChildElements, is_element_processed, void>(
              xml_child_element, context, element_tag))
            return false;
      }
      if (!traversal_control_policy::proceed_to_next_child(context))
        break;
    }
    return true;
  }

  template<BOOST_PP_ENUM_BINARY_PARAMS(6, class ArgRef, = boost::parameter::void_ BOOST_PP_INTERCEPT)>
  struct load_referenced_element
  {
    // struct is needed just because in C++03 function default template parameters are not supported
    template<class XMLElement, class Context>
    static bool load(XMLElement const & xml_element, Context & parent_context)
    {
      typedef typename boost::parameter::parameters<
          boost::parameter::optional<tag::referencing_element, boost::is_base_of<tag::element::any, boost::mpl::_> >
        , boost::parameter::required<tag::expected_elements, boost::mpl::is_sequence<boost::mpl::_> >
        , boost::parameter::optional<tag::ignored_elements, boost::mpl::is_sequence<boost::mpl::_> >
        , boost::parameter::optional<tag::processed_elements, boost::mpl::is_sequence<boost::mpl::_> >
      >::template bind<BOOST_PP_ENUM_PARAMS(6, ArgRef)>::type args;
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
          boost::is_void<processed_elements>,
          typename boost::mpl::if_<
            boost::is_void<ignored_elements>,
            boost::mpl::has_key<boost::mpl::protect<expected_elements>, boost::mpl::_1>,
            boost::mpl::not_<boost::mpl::has_key<boost::mpl::protect<ignored_elements>, boost::mpl::_1> >
          >::type,
          boost::mpl::has_key<boost::mpl::protect<processed_elements>, boost::mpl::_1>
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
    load_element_functor(XMLElement const & xml_element, Context & context)
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
    XMLElement const & xml_element_;
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
  static bool load_child_xml_element(XMLElement const & xml_element, Context & parent_context, ParentElementTag)
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