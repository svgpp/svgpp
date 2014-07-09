#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/traits/attribute_groups.hpp>
#include <svgpp/traits/attribute_type.hpp>
#include <svgpp/traits/attribute_without_parser.hpp>
#include <svgpp/unitless_length_factory.hpp>
#include <svgpp/adapter/list_of_points.hpp>
#include <svgpp/adapter/basic_shapes.hpp>
#include <svgpp/adapter/viewport.hpp>
#include <svgpp/adapter/transform.hpp>
#include <svgpp/adapter/path.hpp>
#include <svgpp/detail/attribute_id_to_tag.hpp>
#include <svgpp/detail/delegate_error_policy.hpp>
#include <svgpp/detail/delegate_load_value_policy.hpp>
#include <svgpp/parser/value_parser.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/empty_base.hpp>
#include <boost/mpl/empty_sequence.hpp>
#include <boost/mpl/single_view.hpp>
#include <boost/mpl/void.hpp>
#include <boost/type_traits.hpp>
#include <boost/tti/has_type.hpp>
#include <boost/parameter.hpp>
#include <boost/preprocessor.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(length_factory)
BOOST_PARAMETER_TEMPLATE_KEYWORD(ignored_attributes)
BOOST_PARAMETER_TEMPLATE_KEYWORD(processed_attributes)
BOOST_PARAMETER_TEMPLATE_KEYWORD(passthrough_attributes)
BOOST_PARAMETER_TEMPLATE_KEYWORD(basic_shapes_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(referencing_element)

template<class Context>
struct context_policy<tag::basic_shapes_policy, Context, void>
{
  static const bool convert_rect_to_path = false;
  static const bool viewport_as_transform = false; // TODO: reorganize basic_shapes_policy
  static const bool calculate_viewport = false;
  static const bool collect_rect_shape_attributes = false;
};

template<class Context>
struct context_policy<tag::ignored_attributes, Context, void>: boost::mpl::set0<>
{
};

template<class Context>
struct context_policy<tag::processed_attributes, Context, void>: boost::mpl::set0<>
{
};

namespace dispatcher_detail
{
  BOOST_PARAMETER_TEMPLATE_KEYWORD(length_factory_holder)

  // To reduce number of instantiations referencing_element_if_needed::type returns ReferencingElementTag
  // only when it matters and 'void' otherwise
  template<class ReferencingElementTag, class ElementTag, class Context, SVGPP_TEMPLATE_ARGS>
  struct referencing_element_if_needed
  {
  private:
    // Unpacking named template parameters
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<::svgpp::tag::basic_shapes_policy>
    >::bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
    typedef typename boost::parameter::value_type<args, ::svgpp::tag::basic_shapes_policy, 
      context_policy<::svgpp::tag::basic_shapes_policy, Context> >::type basic_shapes_policy;

  public:
    typedef typename boost::mpl::if_c<
      (basic_shapes_policy::viewport_as_transform || basic_shapes_policy::calculate_viewport)
      && boost::mpl::has_key<
          viewport_adapter_needs_to_know_referencing_element, 
          boost::mpl::pair<ReferencingElementTag, ElementTag>
        >::type::value,
      ReferencingElementTag,
      void
    >::type type;
  };
}

namespace detail
{
  BOOST_TTI_TRAIT_HAS_TYPE(parser_requires_length_factory, requires_length_factory)

template<class ValueParser, class AttributeTag, class Context, class ValueRange, class PropertySource, class LengthFactory>
inline bool call_value_parser(
  AttributeTag tag, Context & context, ValueRange const & attribute_value, 
  PropertySource property_source,
  LengthFactory const & length_factory,
  typename boost::enable_if<parser_requires_length_factory<ValueParser> >::type * = 0)
{
  return ValueParser::parse(tag, context, attribute_value, property_source, length_factory);
}

template<class ValueParser, class AttributeTag, class Context, class ValueRange, class PropertySource, class LengthFactory>
inline bool call_value_parser(
  AttributeTag tag, Context & context, ValueRange const & attribute_value, 
  PropertySource property_source,
  LengthFactory const &,
  typename boost::disable_if<parser_requires_length_factory<ValueParser> >::type * = 0)
{
  return ValueParser::parse(tag, context, attribute_value, property_source);
}

template<class Loader, class AttributeValue, class PropertySource>
class load_attribute_functor: boost::noncopyable
{
public:
  load_attribute_functor(Loader & loader, AttributeValue const & attributeValue)
    : loader_(loader)
    , attributeValue_(attributeValue)
    , result_(true)
  {
  }

  template<class AttributeTag>
  typename boost::disable_if<typename boost::mpl::apply<typename Loader::is_attribute_processed, AttributeTag>::type>::type
  operator()(AttributeTag) const
  {
  }

  template<class AttributeTag>
  typename boost::enable_if<typename boost::mpl::apply<typename Loader::is_attribute_processed, AttributeTag>::type>::type
  operator()(AttributeTag tag) 
  {
    result_ = loader_.load_attribute_value(tag, attributeValue_, PropertySource());
  }

  bool succeeded() const 
  {
    return result_;
  }

private:
  Loader & loader_;
  AttributeValue const & attributeValue_;
  bool result_;
};

template<class ElementTag, class Length>
class collect_basic_shape_attributes_state
{
  typedef typename collect_basic_shape_attributes_adapter<ElementTag, Length>::type collector_type;
  collector_type collector_;

public:
  collector_type & get_own_context() { return collector_; }

  template<class ErrorPolicy, class Context, class LengthFactory>
  bool on_exit_attributes(Context & context, LengthFactory const & length_factory)
  {
    return collector_.template on_exit_attributesT<
      ErrorPolicy, 
      context_policy<tag::load_value_policy, Context> 
    >(context, length_factory);
  }
};

template<class ElementTag, class Length, class Coordinate, class PathPolicy>
class convert_basic_shape_to_path_state
{
  typedef typename collect_basic_shape_attributes_adapter<ElementTag, Length>::type collector_type;
  collector_type collector_;

public:
  collector_type & get_own_context() { return collector_; }

  template<class ErrorPolicy, class Context, class LengthFactory>
  bool on_exit_attributes(Context & context, LengthFactory const & length_factory)
  {
    typedef path_adapter_if_needed<Context, PathPolicy, Coordinate> adapted_context_type; 
    typename adapted_context_type::holder_type adapted_context(context);

    return collector_.template on_exit_attributesT<
      delegate_error_policy<ErrorPolicy, adapted_context_type>, 
      typename basic_shape_to_path_adapter<ElementTag>::type
    >(adapted_context, length_factory);
  }
};

template<class Length, class Coordinate, class PathPolicy, bool OnlyRoundedRect>
class convert_rect_to_path_state: 
  public convert_basic_shape_to_path_state<tag::element::rect, Length, Coordinate, PathPolicy>
{};

template<class Length, class Coordinate, class PathPolicy>
class convert_rect_to_path_state<Length, Coordinate, PathPolicy, true>
{
  typedef collect_rect_attributes_adapter<Length> collector_type;
  collector_type collector_;

public:
  collector_type & get_own_context() { return collector_; }

  template<class ErrorPolicy, class Context, class LengthFactory>
  bool on_exit_attributes(Context & context, LengthFactory const & length_factory)
  {
    typedef path_adapter_if_needed<Context, PathPolicy, Coordinate> path_context_t; 
    typedef rounded_rect_to_path_adapter<typename path_context_t::type, Context> adapted_context_t;

    typename path_context_t::holder_type path_context(context);
    adapted_context_t adapted_context(path_context, context);

    return collector_.template on_exit_attributesT<
      delegate_error_policy<ErrorPolicy, adapted_context_type>, 
      context_policy<tag::load_value_policy, Context> 
    >(adapted_context, length_factory);
  }
};

template<class ViewportAdapter, class TransformPolicy, class LoadTransformPolicy>
class viewport_transform_state: 
  public ViewportAdapter
{
  typedef ViewportAdapter base_type;
public:
  template<class ErrorPolicy, class Context, class LengthFactory>
  bool on_exit_attributes(Context & context, LengthFactory const & length_factory)
  {
    typedef transform_adapter_if_needed<Context, TransformPolicy, LoadTransformPolicy> adapted_context_type; 
    typename adapted_context_type::holder_type adapted_context(context);

    if (!base_type::on_exit_attributesT<
      viewport_transform_adapter<
        adapted_context_type::load_transform_policy, 
        detail::delegate_load_value_policy<context_policy<tag::load_value_policy, Context>, adapted_context_type> 
      >, 
      detail::delegate_error_policy<ErrorPolicy, adapted_context_type>
    >(adapted_context, length_factory))
      return false;
    adapted_context_type::on_exit_attribute(adapted_context);
    return true;
  }
};

template<class Context, class LengthFactory, class ErrorPolicy>
class on_exit_attributes_functor: boost::noncopyable
{
public:
  on_exit_attributes_functor(Context & context, LengthFactory & length_factory)
    : context_(context)
    , length_factory_(length_factory)
    , result_(true)
  {
  }

  template<typename T>
  void operator()(T & state) const
  {
    result_ = state.on_exit_attributes<ErrorPolicy>(context_, length_factory_) && result_;
  }

  bool succeeded() const 
  {
    return result_;
  }

private:
  Context & context_;
  LengthFactory & length_factory_;
  mutable bool result_; // boost::fusion::for_each doesn't permit non-const functor
};

}

template<class ElementTag, class Context, 
  SVGPP_TEMPLATE_ARGS_DEF>
class attribute_dispatcher;

template<class ElementTag, class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher_base
{
public:
  typedef ElementTag element_tag;
  typedef attribute_dispatcher<ElementTag, Context, SVGPP_TEMPLATE_ARGS_PASS> actual_type; // TODO: review

protected:
  // Unpacking named template parameters
  typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::ignored_attributes, boost::mpl::is_sequence<boost::mpl::_> >
    , boost::parameter::optional<tag::processed_attributes, boost::mpl::is_sequence<boost::mpl::_> >
    , boost::parameter::optional<tag::passthrough_attributes, boost::mpl::is_sequence<boost::mpl::_> >
    , boost::parameter::optional<tag::length_factory>
    , boost::parameter::optional<dispatcher_detail::tag::length_factory_holder>
    , boost::parameter::optional<tag::basic_shapes_policy>
    , boost::parameter::optional<tag::path_policy>
    , boost::parameter::optional<tag::transform_policy>
  >::bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::ignored_attributes, 
    context_policy<tag::ignored_attributes, Context> >::type ignored_attributes;
  typedef typename boost::parameter::value_type<args, tag::processed_attributes, 
    context_policy<tag::processed_attributes, Context> >::type processed_attributes;

  BOOST_STATIC_ASSERT_MSG(boost::mpl::empty<ignored_attributes>::value 
    || boost::mpl::empty<processed_attributes>::value, 
    "Only one of ignored_attributes and processed_attributes may be non-empty");

  typedef typename boost::parameter::value_type<args, tag::length_factory, 
    unitless_length_factory<> >::type length_factory_type;
  typedef typename boost::parameter::value_type<args, dispatcher_detail::tag::length_factory_holder, 
    length_factory_type>::type length_factory_holder; // Can be reference

public:
  typedef typename boost::parameter::value_type<args, tag::number_type, 
    typename context_policy<tag::number_type, Context>::type >::type coordinate_type;
  typedef typename boost::parameter::value_type<args, tag::passthrough_attributes, 
    boost::mpl::set0<> >::type passthrough_attributes;
  typedef typename boost::parameter::value_type<args, tag::basic_shapes_policy, 
    context_policy<tag::basic_shapes_policy, Context> >::type basic_shapes_policy;
  typedef typename boost::parameter::value_type<args, tag::path_policy, 
    context_policy<tag::path_policy, Context> >::type path_policy;
  typedef typename boost::parameter::value_type<args, tag::error_policy, 
    detail::parameter_not_set_tag>::type error_policy_param;

  typedef typename
    boost::mpl::if_<
      boost::mpl::not_<boost::mpl::empty<processed_attributes> >,
      boost::mpl::or_<
        boost::mpl::has_key<boost::mpl::protect<processed_attributes>, boost::mpl::_1>,
        boost::mpl::has_key<boost::mpl::protect<processed_attributes>, boost::mpl::pair<ElementTag, boost::mpl::_1> >
      >,
      boost::mpl::not_<
        boost::mpl::or_<
          boost::mpl::has_key<boost::mpl::protect<ignored_attributes>, boost::mpl::_1>,
          boost::mpl::has_key<boost::mpl::protect<ignored_attributes>, boost::mpl::pair<ElementTag, boost::mpl::_1> >
        > 
      >
    >::type is_attribute_processed;

  attribute_dispatcher_base(Context & context)
    : context_(context)
  {}

  attribute_dispatcher_base(Context & context, length_factory_type & length_factory)
    : context_(context)
    , length_factory_(length_factory)
  {}

  typename length_factory_type & length_factory() 
  { 
    return length_factory_; 
  }

  bool on_exit_attributes()
  {
    return true;
  }

  template<class AttributeValue>
  bool load_attribute(detail::attribute_id id, AttributeValue const & attributeValue, tag::source::attribute)
  {
    detail::load_attribute_functor<actual_type, AttributeValue, tag::source::attribute> fn(
      *static_cast<actual_type *>(this), attributeValue);
    if (!detail::attribute_id_to_tag(element_tag(), id, fn))
      ; // TODO: error
    return fn.succeeded();
  }

  template<class AttributeValue>
  bool load_attribute(detail::attribute_id id, AttributeValue const & attributeValue, tag::source::css)
  {
    detail::load_attribute_functor<actual_type, AttributeValue, tag::source::css> fn(
      *static_cast<actual_type *>(this), attributeValue);
    if (!detail::css_id_to_tag(id, fn))
      ; // TODO: error or assert
    return fn.succeeded();
  }

  template<class AttributeTag, class AttributeValue, class PropertySource>
  bool load_attribute_value(
    AttributeTag attribute_tag, 
    AttributeValue const & attribute_value, 
    PropertySource property_source,
    typename boost::disable_if_c<
      traits::attribute_without_parser<AttributeTag>::value
      || boost::mpl::has_key<passthrough_attributes, AttributeTag>::value>::type * = 0)
  {
    return detail::call_value_parser<value_parser<typename traits::attribute_type<ElementTag, AttributeTag>::type, 
        SVGPP_TEMPLATE_ARGS_PASS> >(attribute_tag, context_, 
      attribute_value, 
      property_source, length_factory_);
  }

  template<class AttributeTag, class AttributeValue, class PropertySource>
  bool load_attribute_value(
    AttributeTag tag, AttributeValue const & attribute_value, 
    PropertySource property_source,
    typename boost::enable_if_c<
      traits::attribute_without_parser<AttributeTag>::value
      || boost::mpl::has_key<passthrough_attributes, AttributeTag>::value>::type * = 0)
  {
    context_policy<tag::load_value_policy, Context>::set(context_, tag, attribute_value, property_source);
    return true;
  }

  template<class EventTag>
  bool notify(EventTag event_tag)
  {
    // Forward notification to original context
    return context_.notify(event_tag);
  }

protected:
  Context & context_;
  length_factory_holder length_factory_;
};

template<class ElementTag, class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher:
  public attribute_dispatcher_base<ElementTag, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef attribute_dispatcher_base<ElementTag, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

public:
  attribute_dispatcher(Context & context)
    : base_type(context)
  {}

  attribute_dispatcher(Context & context, typename base_type::length_factory_type & length_factory)
    : base_type(context, length_factory)
  {}
};

namespace tag { namespace event
{
  struct after_viewport_attributes {};
}}

namespace detail
{

template<class ElementTag, class Context, SVGPP_TEMPLATE_ARGS>
class viewport_attribute_dispatcher:
  public attribute_dispatcher_base<ElementTag, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef attribute_dispatcher_base<ElementTag, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

public:
  viewport_attribute_dispatcher(Context & context)
    : base_type(context)
    , viewport_attributes_applied_(false)
  {}

  viewport_attribute_dispatcher(Context & context, typename base_type::length_factory_type & length_factory)
    : base_type(context, length_factory)
    , viewport_attributes_applied_(false)
  {}

  using base_type::load_attribute_value; 

  template<class AttributeTag, class AttributeValue>
  typename boost::enable_if_c<(base_type::basic_shapes_policy::viewport_as_transform
      || base_type::basic_shapes_policy::calculate_viewport)
    && boost::mpl::has_key<traits::viewport_attributes, AttributeTag>::value, bool>::type
  load_attribute_value(AttributeTag attribute_tag, AttributeValue const & attribute_value, 
                       tag::source::attribute property_source)
  {
    BOOST_ASSERT(!viewport_attributes_applied_);
    return detail::call_value_parser<value_parser<typename traits::attribute_type<ElementTag, AttributeTag>::type, 
        SVGPP_TEMPLATE_ARGS_PASS> >(
      attribute_tag, 
      boost::fusion::at_c<0>(states_), // TODO: change 0 for some meaningful value
      attribute_value, property_source, this->length_factory_);
  }

  using base_type::notify;

  bool notify(tag::event::after_viewport_attributes)
  {
    return on_exit_attributes();
  }

  bool on_exit_attributes()
  {
    if (viewport_attributes_applied_)
      return true;

    viewport_attributes_applied_ = true;
    typedef typename boost::mpl::if_<
      boost::is_same<error_policy_param, detail::parameter_not_set_tag>,
      context_policy<tag::error_policy, Context>,
      error_policy_param
    >::type error_policy_t;

    detail::on_exit_attributes_functor<Context, typename base_type::length_factory_type, error_policy_t> 
      fn(this->context_, this->length_factory_);
    boost::fusion::for_each(states_, fn);
    return fn.succeeded();
  }

private:
  typedef typename boost::parameter::parameters<
    boost::parameter::optional<tag::transform_policy>,
    boost::parameter::optional<tag::load_transform_policy>,
    boost::parameter::optional<tag::referencing_element>
  >::bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::transform_policy, 
    context_policy<tag::transform_policy, Context> >::type transform_policy;
  typedef typename boost::parameter::value_type<args, tag::load_transform_policy, 
    context_policy<tag::load_transform_policy, Context> >::type load_transform_policy;
  typedef typename boost::parameter::value_type<args, tag::referencing_element, 
    void>::type referencing_element;
  
  typedef calculate_viewport_adapter<
    typename base_type::length_factory_type::length_type, 
    typename base_type::coordinate_type,
    typename get_viewport_size_source<referencing_element, ElementTag>::type
  > viewport_adapter;
  typedef typename boost::mpl::if_c< 
    base_type::basic_shapes_policy::viewport_as_transform,
    boost::mpl::single_view<
      detail::viewport_transform_state<
        viewport_adapter,
        typename transform_policy,
        typename load_transform_policy
      > 
    >,
    typename boost::mpl::if_c<base_type::basic_shapes_policy::calculate_viewport,
      boost::mpl::single_view<viewport_adapter>,
      boost::mpl::empty_sequence>::type
  >::type state_types_sequence;
  typedef typename boost::fusion::result_of::as_vector<state_types_sequence>::type state_types;

  state_types states_;
  bool viewport_attributes_applied_;
};

}

template<class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher<tag::element::svg, Context, SVGPP_TEMPLATE_ARGS_PASS>:
  public detail::viewport_attribute_dispatcher<tag::element::svg, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef detail::viewport_attribute_dispatcher<tag::element::svg, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

public:
  attribute_dispatcher(Context & context)
    : base_type(context)
  {}

  attribute_dispatcher(Context & context, typename base_type::length_factory_type & length_factory)
    : base_type(context, length_factory)
  {}
};

template<class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher<tag::element::symbol, Context, SVGPP_TEMPLATE_ARGS_PASS>:
  public detail::viewport_attribute_dispatcher<tag::element::symbol, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef detail::viewport_attribute_dispatcher<tag::element::symbol, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

public:
  attribute_dispatcher(Context & context)
    : base_type(context)
  {}

  attribute_dispatcher(Context & context, typename base_type::length_factory_type & length_factory)
    : base_type(context, length_factory)
  {}
};

namespace detail
{

template<class ElementTag, class Context, SVGPP_TEMPLATE_ARGS>
class basic_shape_attribute_dispatcher:
  public attribute_dispatcher_base<ElementTag, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef attribute_dispatcher_base<ElementTag, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;
  typedef typename base_type::length_factory_type::length_type length_type;
  typedef typename boost::mpl::if_< 
    boost::mpl::has_key<typename base_type::basic_shapes_policy::convert_to_path, ElementTag>,
    boost::mpl::single_view<
      typename boost::mpl::if_<
        boost::is_same<ElementTag, tag::element::rect>,
        convert_rect_to_path_state<
          length_type, 
          typename base_type::coordinate_type, 
          typename base_type::path_policy, 
          base_type::basic_shapes_policy::convert_only_rounded_rect_to_path
        >, 
        convert_basic_shape_to_path_state<
          ElementTag,
          length_type, 
          typename base_type::coordinate_type, 
          typename base_type::path_policy
        >
      >::type
    >,
    typename boost::mpl::if_< 
      boost::mpl::has_key<typename base_type::basic_shapes_policy::collect_attributes, ElementTag>,
      boost::mpl::single_view<collect_basic_shape_attributes_state<ElementTag, length_type> >,
      boost::mpl::empty_sequence
    >::type
  >::type state_types_sequence;
  typedef typename boost::fusion::result_of::as_vector<state_types_sequence>::type state_types;

  state_types states_;

public:
  basic_shape_attribute_dispatcher(Context & context)
    : base_type(context)
  {
  }

  basic_shape_attribute_dispatcher(Context & context, typename base_type::length_factory_type & length_factory)
    : base_type(context, length_factory)
  {}

  bool on_exit_attributes()
  {
    typedef typename boost::mpl::if_<
      boost::is_same<error_policy_param, parameter_not_set_tag>,
      context_policy<tag::error_policy, Context>,
      error_policy_param
    >::type error_policy_t;

    on_exit_attributes_functor<Context, typename base_type::length_factory_type, error_policy_t> 
      fn(this->context_, this->length_factory_);
    boost::fusion::for_each(states_, fn);
    return fn.succeeded();
  }

  using base_type::load_attribute_value; 

  template<class AttributeValue, class AttributeTag>
  typename boost::enable_if_c<
    boost::mpl::has_key<typename base_type::basic_shapes_policy::convert_to_path, ElementTag>::value
    && boost::mpl::has_key<typename basic_shape_attributes<ElementTag>::type, AttributeTag>::value, bool>::type
  load_attribute_value(AttributeTag attribute_tag, 
                       AttributeValue const & attribute_value, 
                       tag::source::attribute property_source)
  {
    return call_value_parser<
      value_parser<
        typename traits::attribute_type<ElementTag, AttributeTag>::type, 
        SVGPP_TEMPLATE_ARGS_PASS
      > 
    >(
      attribute_tag, 
      //boost::fusion::at_key<collect_attributes_adapter>(states_), 
      boost::fusion::at_c<0>(states_).get_own_context(), // TODO: change 0 for some meaningful value
      attribute_value, property_source, this->length_factory_);
  }
};

} // namespace detail

template<class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher<tag::element::rect, Context, SVGPP_TEMPLATE_ARGS_PASS>:
  public detail::basic_shape_attribute_dispatcher<tag::element::rect, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef detail::basic_shape_attribute_dispatcher<tag::element::rect, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

public:
  attribute_dispatcher(Context & context)
    : base_type(context)
  {}

  attribute_dispatcher(Context & context, typename base_type::length_factory_type & length_factory)
    : base_type(context, length_factory)
  {}
};

template<class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher<tag::element::circle, Context, SVGPP_TEMPLATE_ARGS_PASS>:
  public detail::basic_shape_attribute_dispatcher<tag::element::circle, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef detail::basic_shape_attribute_dispatcher<tag::element::circle, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

public:
  attribute_dispatcher(Context & context)
    : base_type(context)
  {}

  attribute_dispatcher(Context & context, typename base_type::length_factory_type & length_factory)
    : base_type(context, length_factory)
  {}
};

template<class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher<tag::element::ellipse, Context, SVGPP_TEMPLATE_ARGS_PASS>:
  public detail::basic_shape_attribute_dispatcher<tag::element::ellipse, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
typedef detail::basic_shape_attribute_dispatcher<tag::element::ellipse, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

public:
  attribute_dispatcher(Context & context)
    : base_type(context)
  {}

  attribute_dispatcher(Context & context, typename base_type::length_factory_type & length_factory)
    : base_type(context, length_factory)
  {}
};

template<class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher<tag::element::line, Context, SVGPP_TEMPLATE_ARGS_PASS>:
  public detail::basic_shape_attribute_dispatcher<tag::element::line, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
typedef detail::basic_shape_attribute_dispatcher<tag::element::line, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

public:
  attribute_dispatcher(Context & context)
    : base_type(context)
  {}

  attribute_dispatcher(Context & context, typename base_type::length_factory_type & length_factory)
    : base_type(context, length_factory)
  {}
};

template<class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher<tag::element::polyline, Context, SVGPP_TEMPLATE_ARGS_PASS>:
  public attribute_dispatcher_base<tag::element::polyline, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef attribute_dispatcher_base<tag::element::polyline, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

public:
  attribute_dispatcher(Context & context)
    : base_type(context)
  {
  }

  attribute_dispatcher(Context & context, typename base_type::length_factory_type & length_factory)
    : base_type(context, length_factory)
  {}

  using base_type::load_attribute_value; 

  template<class AttributeValue>
  typename boost::enable_if_c<
    !boost::is_same<AttributeValue, void>::value // Just to make type dependend on template parameter
    && base_type::basic_shapes_policy::polyline_as_path, bool>::type
  load_attribute_value(tag::attribute::points attribute_tag, AttributeValue const & attribute_value, 
                       tag::source::attribute property_source)
  {
    list_of_points_to_path_adapter<Context> adapter(this->context_);
    return detail::call_value_parser<value_parser<traits::attribute_type<tag::element::polyline, tag::attribute::points>::type, 
        SVGPP_TEMPLATE_ARGS_PASS> >(
      attribute_tag, adapter,
      attribute_value, property_source, this->length_factory_);
  }
};

}
