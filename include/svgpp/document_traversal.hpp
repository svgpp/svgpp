#pragma once

#include <svgpp/attribute_traversal.hpp>
#include <svgpp/attribute_dispatcher.hpp>
#include <svgpp/template_parameters.hpp>
#include <svgpp/context_policy_load_text.hpp>
#include <svgpp/detail/element_id_to_tag.hpp>
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

  template<class XMLElement, class LoaderState>
  same(ParentContext & context, XMLElement const &, LoaderState &)
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

  template<class XMLElement, class LoaderState>
  on_stack(ParentContext & context, XMLElement const &, LoaderState &)
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
  get_ptr_from_parent(ParentContext & context, XMLElement const &, LoaderState &)
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
BOOST_PARAMETER_TEMPLATE_KEYWORD(xml_element_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(ignored_elements)
BOOST_PARAMETER_TEMPLATE_KEYWORD(processed_elements)

template<SVGPP_TEMPLATE_ARGS_DEF>
class document_traversal
{
protected:
  typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::context_factories>
    , boost::parameter::optional<tag::xml_element_policy>
    , boost::parameter::optional<tag::load_text_policy>
    , boost::parameter::optional<tag::error_policy>
    , boost::parameter::optional<tag::length_factory>
    , boost::parameter::optional<tag::ignored_elements, boost::mpl::is_sequence<boost::mpl::_> >
    , boost::parameter::optional<tag::processed_elements, boost::mpl::is_sequence<boost::mpl::_> >
  >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::context_factories, 
    default_context_factories>::type context_factories;
  typedef typename boost::parameter::value_type<args, tag::xml_element_policy, 
    detail::parameter_not_set_tag>::type xml_element_policy_param;
  typedef typename boost::parameter::value_type<args, tag::load_text_policy, 
    detail::parameter_not_set_tag>::type load_text_policy_param;
  typedef typename boost::parameter::value_type<args, tag::ignored_elements, 
    boost::mpl::empty_sequence>::type ignored_elements;
  typedef typename boost::parameter::value_type<args, tag::processed_elements, 
    boost::mpl::empty_sequence>::type processed_elements;
  typedef typename boost::parameter::value_type<args, tag::error_policy, 
    detail::parameter_not_set_tag>::type error_policy_param;
  typedef typename boost::parameter::value_type<args, tag::length_factory, 
    unitless_length_factory<> >::type length_factory_type;

  BOOST_STATIC_ASSERT_MSG(boost::mpl::empty<ignored_elements>::value 
    || boost::mpl::empty<processed_elements>::value, "Only one of ignored_elements and processed_elements may be non-empty");

  typedef typename
    boost::mpl::if_<
      boost::mpl::not_<boost::mpl::empty<processed_elements> >,
      boost::mpl::has_key<boost::mpl::protect<processed_elements>, boost::mpl::_1>,
      boost::mpl::not_<boost::mpl::has_key<boost::mpl::protect<ignored_elements>, boost::mpl::_1> >  
    >::type is_element_processed;

  struct state_holder
  {
    length_factory_type & length_factory() { return length_factory_; }

  private:
    length_factory_type length_factory_;
  };

public:
  template<class XMLElement, class Context>
  static bool load_document(XMLElement & xml_element_svg, Context & context)
  {
    return load_element<traits::child_element_types<tag::element::svg>::type, void>
      (xml_element_svg, context, tag::element::svg());
  }

  template<class ExpectedChildElements, class ReferencingElement, class XMLElement, class Context, class ElementTag>
  static bool load_element(XMLElement & xml_element, Context & context, ElementTag element_tag, 
    state_holder state_copy = state_holder())
  {
    return load_attributes<ReferencingElement>(xml_element, context, element_tag, state_copy)
      && load_element_content<ExpectedChildElements>(xml_element, context, element_tag, state_copy);
  }

  template<class ReferencingElement, class XMLElement, class Context, class ElementTag>
  static bool load_attributes(XMLElement & xml_element, Context & context, ElementTag element_tag, 
    state_holder & state)
  {
    typedef typename boost::mpl::if_<
      boost::is_same<xml_element_policy_param, detail::parameter_not_set_tag>,
      xml_element_iterator_policy<XMLElement>,
      xml_element_policy_param
    >::type xml_policy_t;

    typedef typename dispatcher_detail::referencing_element_if_needed<
      ReferencingElement,
      ElementTag,
      Context,
      SVGPP_TEMPLATE_ARGS_PASS
    >::type referencing_element_tag;
    typedef attribute_dispatcher<
      ElementTag, 
      Context, 
      dispatcher_detail::length_factory_holder<length_factory_type&>,
      referencing_element<referencing_element_tag>,
      SVGPP_TEMPLATE_ARGS_PASS
    > attribute_dispatcher_t;
    static const bool parse_style_attribute = 
      !boost::mpl::has_key<attribute_dispatcher_t::passthrough_attributes, tag::attribute::style>::value
      && boost::mpl::apply<typename attribute_dispatcher_t::is_attribute_processed, tag::attribute::style>::type::value;
    attribute_dispatcher_t attribute_dispatcher(context, state.length_factory());
    if (!attribute_traversal<
        ElementTag,
        parse_style_attribute,
        SVGPP_TEMPLATE_ARGS_PASS
      >::type::load(xml_policy_t::get_attributes(xml_element), attribute_dispatcher))
      return false;
    return attribute_dispatcher.on_exit_attributes();
  }

  template<class ExpectedChildElements, class ReferencingElement, class XMLElement, class ParentContext, class ElementTag>
  static bool load_child_element(XMLElement & xml_element, ParentContext & parent_context, ElementTag element_tag, 
    state_holder const & state)
  {
    typedef typename context_factories::template apply<ParentContext, ElementTag>::type context_factory_t;
    context_factory_t context_factory(parent_context, xml_element, state);
    if (!load_element<ExpectedChildElements, ReferencingElement>(xml_element, context_factory.get(), element_tag, state))
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
  load_element_content(XMLElement const &, Context const &, ElementTag, state_holder const &)
  {
    return true;
  }

  template<class ExpectedChildElements, class XMLElement, class Context, class ElementTag>
  static typename boost::disable_if<
    boost::mpl::or_<
      boost::mpl::empty<typename traits::child_element_types<ElementTag>::type>,
      traits::element_with_text_content<ElementTag> >, bool>::type
  load_element_content(XMLElement & xml_element, Context & context, ElementTag element_tag, 
    state_holder const & state)
  {
    typedef typename boost::mpl::if_<
      boost::is_same<xml_element_policy_param, detail::parameter_not_set_tag>,
      xml_element_iterator_policy<XMLElement>,
      xml_element_policy_param
    >::type xml_policy_t;
    typedef typename boost::mpl::if_<
      boost::is_same<error_policy_param, detail::parameter_not_set_tag>,
      context_policy<tag::error_policy, Context>,
      error_policy_param
    >::type error_policy;

    for(typename xml_policy_t::iterator_type xml_child_element = xml_policy_t::get_child_elements(xml_element); 
      !xml_policy_t::is_end(xml_child_element); xml_policy_t::advance_element(xml_child_element))
    {
      if (!load_child_xml_element<ExpectedChildElements, is_element_processed, void>(
          xml_child_element, context, element_tag, state))
        return false;
    }
    return true;
  }

  template<class ExpectedChildElements, class XMLElement, class Context, class ElementTag>
  static typename boost::enable_if<traits::element_with_text_content<ElementTag>, bool>::type
  load_element_content(XMLElement & xml_element, Context & context, ElementTag element_tag, 
    state_holder const & state)
  {
    typedef typename boost::mpl::if_<
      boost::is_same<xml_element_policy_param, detail::parameter_not_set_tag>,
      xml_element_iterator_policy<XMLElement>,
      xml_element_policy_param
    >::type xml_policy_t;
    typedef typename boost::mpl::if_<
      boost::is_same<load_text_policy_param, detail::parameter_not_set_tag>,
      context_policy<tag::load_text_policy, Context>,
      load_text_policy_param
    >::type load_text_policy;
    
    for(typename xml_policy_t::iterator_type xml_child_element = xml_policy_t::get_child_elements_and_texts(xml_element); 
      !xml_policy_t::is_end(xml_child_element); xml_policy_t::advance_element_or_text(xml_child_element))
    {
      if (xml_policy_t::is_text(xml_child_element))
        load_text_policy::set_text(xml_policy_t::get_text(xml_child_element));
      else
      {
        if (!load_child_xml_element<ExpectedChildElements, is_element_processed, false>(
            xml_child_element, context, element_tag, state))
          return false;
      }
    }
    return true;
  }

  template<
    class ReferencingElement,   
    class ExpectedElements,     // Elements other than ExpectedElements will cause unexpected_element error
    class ProcessedElements,    // ignored_elements<Sequence> or processed_elements<Sequence> - override
                                // class settings for the referenced element.
                                // E.g. "symbol" is processed only when referenced by "use"
    class XMLElement, 
    class Context
  >
  static bool load_referenced_element(XMLElement & xml_element, Context & parent_context, 
    state_holder const & state = state_holder())
  {
    typedef typename boost::parameter::parameters<
        boost::parameter::optional<tag::ignored_elements, boost::mpl::is_sequence<boost::mpl::_> >
      , boost::parameter::optional<tag::processed_elements, boost::mpl::is_sequence<boost::mpl::_> >
    >::template bind<ProcessedElements>::type args;
    typedef typename boost::parameter::value_type<args, tag::ignored_elements, 
      boost::mpl::empty_sequence>::type ignored_elements;
    typedef typename boost::parameter::value_type<args, tag::processed_elements, 
      boost::mpl::empty_sequence>::type processed_elements;

    BOOST_STATIC_ASSERT_MSG(!boost::mpl::empty<ignored_elements>::value 
      || !boost::mpl::empty<processed_elements>::value, "One of ignored_elements and processed_elements must be non-empty");

    typedef typename
      boost::mpl::if_<
        boost::mpl::not_<boost::mpl::empty<processed_elements> >,
        boost::mpl::has_key<boost::mpl::protect<processed_elements>, boost::mpl::_1>,
        boost::mpl::not_<boost::mpl::has_key<boost::mpl::protect<ignored_elements>, boost::mpl::_1> >  
      >::type is_element_processed;

    // The 'a' element may contain any element that its parent may contain, except itself.
    // So we can't handle 'a' without knowing its parent.
    BOOST_MPL_ASSERT_NOT((boost::mpl::has_key<ExpectedElements, tag::element::a>));
    // TODO: Pass ReferencingElement only when needed, to reduce number of instantiations
    return load_child_xml_element<
        ExpectedElements, 
        is_element_processed, 
        ReferencingElement
      >(xml_element, parent_context, 
        boost::mpl::void_(), // ParentElementTag parameter can be of any type, it shouldn't be used for
                             // any element except 'a'
        state);
  }

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
    load_element_functor(XMLElement & xml_element, Context & context, state_holder const & state)
      : xml_element_(xml_element)
      , context_(context)
      , state_(state)
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
        traits::child_element_types<ElementTag>::type, 
        ReferencingElement
      >(xml_element_, context_, tag, state_);
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
        boost::mpl::erase_key<
          traits::child_element_types<ParentElementTag>::type,
          tag::element::a
        >::type,
        ReferencingElement
      >(xml_element_, context_, tag, state_);
    }

    template<class ElementTag>
    typename boost::disable_if<typename boost::mpl::apply<IsElementProcessed, ElementTag>::type>::type
    operator()(ElementTag) const
    {
    }

    bool succeeded() const 
    { 
      return result_; 
    }

  private:
    XMLElement & xml_element_;
    Context & context_;
    state_holder const & state_;
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
  static bool load_child_xml_element(XMLElement & xml_element, Context & parent_context, ParentElementTag,
    state_holder const & state)
  {
    typedef typename boost::mpl::if_<
      boost::is_same<xml_element_policy_param, detail::parameter_not_set_tag>,
      xml_element_iterator_policy<XMLElement>,
      xml_element_policy_param
    >::type xml_policy_t;
    typedef typename boost::mpl::if_<
      boost::is_same<error_policy_param, detail::parameter_not_set_tag>,
      context_policy<tag::error_policy, Context>,
      error_policy_param
    >::type error_policy;

    xml_policy_t::element_name_type element_name = xml_policy_t::get_local_name(xml_element);
    detail::element_type_id element_type_id = detail::element_name_to_id_dictionary::find(
      xml_policy_t::get_string_range(element_name));
    if (element_type_id != detail::unknown_element_type_id)
    {
      load_element_functor<XMLElement, Context, IsElementProcessed, ParentElementTag, ReferencingElement> load_functor
        (xml_element, parent_context, state);
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