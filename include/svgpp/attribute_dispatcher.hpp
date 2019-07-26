// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/adapter/basic_shapes.hpp>
#include <svgpp/adapter/list_of_points.hpp>
#include <svgpp/adapter/marker_viewport.hpp>
#include <svgpp/adapter/path.hpp>
#include <svgpp/adapter/transform.hpp>
#include <svgpp/adapter/viewport.hpp>
#include <svgpp/detail/adapt_context.hpp>
#include <svgpp/detail/attribute_id_to_tag.hpp>
#include <svgpp/parser/value_parser.hpp>
#include <svgpp/policy/basic_shapes.hpp>
#include <svgpp/policy/notify.hpp>
#include <svgpp/policy/viewport.hpp>
#include <svgpp/traits/attribute_groups.hpp>
#include <svgpp/traits/attribute_type.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/fusion/include/accumulate.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/empty_base.hpp>
#include <boost/mpl/empty_sequence.hpp>
#include <boost/mpl/single_view.hpp>
#include <boost/mpl/void.hpp>
#include <boost/type_traits.hpp>
#include <boost/parameter.hpp>
#include <boost/preprocessor.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(basic_shapes_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(ignored_attributes)
BOOST_PARAMETER_TEMPLATE_KEYWORD(processed_attributes)
BOOST_PARAMETER_TEMPLATE_KEYWORD(passthrough_attributes)
BOOST_PARAMETER_TEMPLATE_KEYWORD(referencing_element)
BOOST_PARAMETER_TEMPLATE_KEYWORD(viewport_policy)

namespace dispatcher_detail
{
  // To reduce number of instantiations referencing_element_if_needed::type returns ReferencingElementTag
  // only when it matters and 'void' otherwise
  template<class ReferencingElementTag, class ElementTag, class Context, SVGPP_TEMPLATE_ARGS2>
  struct referencing_element_if_needed
  {
  private:
    // Unpacking named template parameters
    typedef typename boost::parameter::parameters<
      boost::parameter::optional< ::svgpp::tag::viewport_policy>
    >::bind<SVGPP_TEMPLATE_ARGS2_PASS>::type args;
    typedef typename boost::parameter::value_type<args, ::svgpp::tag::viewport_policy, 
      typename policy::viewport::by_context<Context>::type>::type viewport_policy;

  public:
    typedef typename boost::mpl::if_c<
      viewport_policy::calculate_viewport
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

template<class Loader, class AttributeValue, class PropertySource>
class load_attribute_functor: boost::noncopyable
{
public:
  load_attribute_functor(Loader & loader, AttributeValue const & attributeValue)
    : loader_(loader)
    , attributeValue_(attributeValue)
    , result_(true)
  {}

  template<class AttributeTag>
  typename boost::disable_if<typename boost::mpl::apply<typename Loader::is_attribute_processed, AttributeTag>::type>::type
  operator()(AttributeTag) const
  {}

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

  template<class Context>
  bool on_exit_attributes(Context & context)
  {
    return collector_.on_exit_attributes(context);
  }
};

template<
  class ElementTag, 
  class Length, 
  class ShapeToPathAdapter = typename basic_shape_to_path_adapter<ElementTag>::type
>
class convert_basic_shape_to_path_state
{
  typedef typename collect_basic_shape_attributes_adapter<ElementTag, Length>::type collector_type;
  collector_type collector_;

public:
  collector_type & get_own_context() { return collector_; }

  template<class Context>
  bool on_exit_attributes(Context & context)
  {
    typedef path_adapter_if_needed<Context> path_adapter_t; 
    typename path_adapter_t::type path_adapter(detail::unwrap_context<Context, tag::path_events_policy>::get(context));

    return collector_.on_exit_attributes(
      detail::adapt_context<tag::basic_shapes_events_policy, ShapeToPathAdapter>(context, path_adapter_t::adapt_context(context, path_adapter)));
  }
};

template<class Length, class ShapeToPathAdapter>
class convert_basic_shape_to_path_state<tag::element::line, Length, ShapeToPathAdapter>
{
  typedef typename collect_basic_shape_attributes_adapter<tag::element::line, Length>::type collector_type;
  collector_type collector_;

public:
  collector_type & get_own_context() { return collector_; }

  template<class Context>
  bool on_exit_attributes(Context & context)
  {
    typedef path_adapter_if_needed<Context> path_adapter_t; 
    typedef path_markers_adapter_if_needed<typename path_adapter_t::adapted_context> markers_adapter_t;
    typename path_adapter_t::type path_adapter(detail::unwrap_context<Context, tag::path_events_policy>::get(context));
    typename path_adapter_t::adapted_context_holder adapted_path_context(path_adapter_t::adapt_context(context, path_adapter));
    typename markers_adapter_t::type markers_adapter(adapted_path_context);

    return collector_.on_exit_attributes(
      detail::adapt_context<tag::basic_shapes_events_policy, ShapeToPathAdapter>(context, markers_adapter_t::adapt_context(adapted_path_context, markers_adapter)));
  }
};

template<class Length>
class convert_rounded_rect_to_path_state
  : public convert_basic_shape_to_path_state<tag::element::rect, Length, rounded_rect_to_path_adapter>
{};

template<class ViewportAdapter>
class viewport_transform_state: 
  public ViewportAdapter
{
  typedef ViewportAdapter base_type;
public:
  template<class Context>
  bool on_exit_attributes(Context & context)
  {
    typedef transform_adapter_if_needed<Context> transform_adapter_t; 
    typename transform_adapter_t::type transform_adapter(
      detail::unwrap_context<Context, tag::transform_events_policy>::get(context));

    if (!base_type::on_exit_attributes(
      detail::adapt_context<tag::viewport_events_policy, viewport_transform_adapter>(
        context, 
        transform_adapter_t::adapt_context(context, transform_adapter))))
      return false;
    transform_adapter_t::on_exit_attribute(transform_adapter);
    return true;
  }
};

template<class Context, SVGPP_TEMPLATE_ARGS>
class on_exit_attributes_functor
{
public:
  typedef bool result_type;

  on_exit_attributes_functor(Context & context)
    : context_(context)
  {}

  template<typename T>
  bool operator()(bool previous_succeeded, T & state) const
  {
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::error_policy>,
      boost::parameter::optional<tag::length_policy>,
      boost::parameter::optional<tag::path_policy>,
      boost::parameter::optional<tag::path_events_policy>,
      boost::parameter::optional<tag::markers_policy>,
      boost::parameter::optional<tag::marker_events_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args_t;

    return state.on_exit_attributes(detail::bind_context_parameters<args_t>(context_)) 
      && previous_succeeded;
  }

private:
  Context & context_;
};

}

template<class ElementTag, class Context, 
  SVGPP_TEMPLATE_ARGS_DEF>
class attribute_dispatcher;

template<class ElementTag, class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher_base: boost::noncopyable
{
public:
  typedef Context context_type;
  typedef ElementTag element_tag;
  typedef attribute_dispatcher<ElementTag, Context, SVGPP_TEMPLATE_ARGS_PASS> actual_type; 

protected:
  // Unpacking named template parameters
  typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::ignored_attributes, boost::mpl::is_sequence<boost::mpl::_> >
    , boost::parameter::optional<tag::processed_attributes, boost::mpl::is_sequence<boost::mpl::_> >
    , boost::parameter::optional<tag::passthrough_attributes, boost::mpl::is_sequence<boost::mpl::_> >
    , boost::parameter::optional<tag::length_policy>
    , boost::parameter::optional<tag::basic_shapes_policy>
    , boost::parameter::optional<tag::number_type>
  >::bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::ignored_attributes, void>::type ignored_attributes;
  typedef typename boost::parameter::value_type<args, tag::processed_attributes, void>::type processed_attributes;

  BOOST_STATIC_ASSERT_MSG(boost::is_void<ignored_attributes>::value 
    != boost::is_void<processed_attributes>::value, 
    "Only one of ignored_attributes and processed_attributes must be set");

  typedef typename detail::unwrap_context<Context, tag::length_policy>::template bind<args>::type::length_factory_type length_factory_type;
  
public:
  typedef typename boost::parameter::value_type<args, tag::number_type, 
    typename number_type_by_context<Context>::type>::type coordinate_type;
  typedef typename boost::parameter::value_type<args, tag::passthrough_attributes, 
    boost::mpl::set0<> >::type passthrough_attributes;

  typedef typename
    boost::mpl::if_<
      boost::is_void<ignored_attributes>,
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

  Context & context()
  {
    return context_;
  }

  bool on_exit_attributes()
  {
    return true;
  }

  template<class AttributeValue>
  bool load_attribute(detail::attribute_id id, AttributeValue const & attributeValue, tag::source::attribute source)
  {
    detail::load_attribute_functor<actual_type, AttributeValue, tag::source::attribute> fn(
      *static_cast<actual_type *>(this), attributeValue);
    if (!detail::attribute_id_to_tag(element_tag(), id, fn))
    {
      typedef typename boost::parameter::parameters<
          boost::parameter::optional<tag::error_policy>
      >::bind<SVGPP_TEMPLATE_ARGS_PASS>::type args_t;
      typedef typename boost::parameter::value_type<args_t, tag::error_policy, 
        policy::error::default_policy<Context> >::type error_policy_t;
      error_policy_t::unexpected_attribute(context_, id, source);
    }
    return fn.succeeded();
  }

  template<class AttributeValue>
  bool load_attribute(detail::attribute_id id, AttributeValue const & attributeValue, tag::source::css)
  {
    detail::load_attribute_functor<actual_type, AttributeValue, tag::source::css> fn(
      *static_cast<actual_type *>(this), attributeValue);
    if (!detail::css_id_to_tag(id, fn))
      BOOST_ASSERT(false);
    return fn.succeeded();
  }

  template<class AttributeTag, class AttributeValue, class PropertySource>
  bool load_attribute_value(
    AttributeTag attribute_tag, 
    AttributeValue const & attribute_value, 
    PropertySource property_source,
    typename boost::disable_if_c<
      boost::is_same<typename traits::attribute_type<ElementTag, AttributeTag>::type, tag::type::string>::value
      || boost::mpl::has_key<passthrough_attributes, AttributeTag>::value>::type * = 0)
  {
    return value_parser<typename traits::attribute_type<ElementTag, AttributeTag>::type, 
        SVGPP_TEMPLATE_ARGS_PASS>::parse(attribute_tag, context_, 
      attribute_value, 
      property_source);
  }

  template<class AttributeTag, class AttributeValue, class PropertySource>
  bool load_attribute_value(
    AttributeTag tag, AttributeValue const & attribute_value, 
    PropertySource property_source,
    typename boost::enable_if_c<
      boost::is_same<typename traits::attribute_type<ElementTag, AttributeTag>::type, tag::type::string>::value
      || boost::mpl::has_key<passthrough_attributes, AttributeTag>::value>::type * = 0)
  {
    policy::value_events::default_policy<Context>::set(context_, tag, property_source, attribute_value);
    return true;
  }

  template<class EventTag>
  bool notify(EventTag event_tag)
  {
    // Forward notification to original context
    return policy::notify::default_policy<Context>::notify(context_, event_tag);
  }

protected:
  Context & context_;
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

  typedef typename boost::parameter::parameters<
    boost::parameter::optional<tag::referencing_element>,
    boost::parameter::optional<tag::viewport_policy>
  >::bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::referencing_element, 
    void>::type referencing_element;
  typedef typename boost::parameter::value_type<args, tag::viewport_policy, 
    typename policy::viewport::by_context<Context>::type>::type viewport_policy;

  static const bool calculate_viewport = 
    boost::is_same<ElementTag, tag::element::pattern>::value
      ? viewport_policy::calculate_pattern_viewport
      : viewport_policy::calculate_viewport;

public:
  viewport_attribute_dispatcher(Context & context)
    : base_type(context)
    , viewport_attributes_applied_(false)
  {}

  using base_type::load_attribute_value; 

  template<class AttributeTag, class AttributeValue>
  typename boost::enable_if_c<
    calculate_viewport
    && boost::mpl::has_key<traits::viewport_attributes, AttributeTag>::value, bool>::type
  load_attribute_value(AttributeTag attribute_tag, AttributeValue const & attribute_value, 
                       tag::source::attribute property_source)
  {
    BOOST_ASSERT(!viewport_attributes_applied_);
    return value_parser<typename traits::attribute_type<ElementTag, AttributeTag>::type, 
        SVGPP_TEMPLATE_ARGS_PASS>::parse(
      attribute_tag, 
      detail::adapt_context_value_events(this->context_, boost::fusion::at_c<0>(states_)),
      attribute_value, property_source);
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

    detail::on_exit_attributes_functor<Context, SVGPP_TEMPLATE_ARGS_PASS> fn(this->context_);
    return boost::fusion::accumulate(states_, true, fn);
  }

private:
  typedef calculate_viewport_adapter<
    ElementTag,
    referencing_element,
    typename base_type::length_factory_type::length_type, 
    typename base_type::coordinate_type
  > viewport_adapter;
  typedef 
    typename boost::mpl::if_c<
      calculate_viewport,
      typename boost::mpl::if_c< 
        viewport_policy::viewport_as_transform,
        boost::mpl::single_view<
          detail::viewport_transform_state<viewport_adapter> 
        >,
        boost::mpl::single_view<viewport_adapter>
      >::type,
      boost::mpl::empty_sequence
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
};

template<class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher<tag::element::pattern, Context, SVGPP_TEMPLATE_ARGS_PASS>:
  public detail::viewport_attribute_dispatcher<tag::element::pattern, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef detail::viewport_attribute_dispatcher<tag::element::pattern, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

public:
  attribute_dispatcher(Context & context)
    : base_type(context)
  {}
};

template<class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher<tag::element::marker, Context, SVGPP_TEMPLATE_ARGS_PASS>:
  public attribute_dispatcher_base<tag::element::marker, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef attribute_dispatcher_base<tag::element::marker, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

  typedef typename boost::parameter::parameters<
    boost::parameter::optional<tag::viewport_policy>
  >::bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::viewport_policy, 
    typename policy::viewport::by_context<Context>::type>::type viewport_policy;

public:
  attribute_dispatcher(Context & context)
    : base_type(context)
    , viewport_attributes_applied_(false)
  {}

  using base_type::load_attribute_value; 

  template<class AttributeTag, class AttributeValue>
  typename boost::enable_if_c<
    viewport_policy::calculate_marker_viewport
    && boost::mpl::has_key<traits::marker_viewport_attributes, AttributeTag>::value, bool>::type
  load_attribute_value(AttributeTag attribute_tag, AttributeValue const & attribute_value, 
                       tag::source::attribute property_source)
  {
    BOOST_ASSERT(!viewport_attributes_applied_);
    return value_parser<typename traits::attribute_type<tag::element::marker, AttributeTag>::type, 
        SVGPP_TEMPLATE_ARGS_PASS>::parse(
      attribute_tag, 
      detail::adapt_context_value_events(this->context_, boost::fusion::at_c<0>(states_)),
      attribute_value, property_source);
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

    detail::on_exit_attributes_functor<Context, SVGPP_TEMPLATE_ARGS_PASS> fn(this->context_);
    return boost::fusion::accumulate(states_, true, fn);
  }

private:
  typedef calculate_marker_viewport_adapter<
    typename base_type::length_factory_type::length_type, 
    typename base_type::coordinate_type
  > viewport_adapter;
  typedef 
    typename boost::mpl::if_c<
      viewport_policy::calculate_marker_viewport,
      typename boost::mpl::if_c< 
        viewport_policy::viewport_as_transform,
        boost::mpl::single_view<
          detail::viewport_transform_state<viewport_adapter> 
        >,
        boost::mpl::single_view<viewport_adapter>
      >::type,
      boost::mpl::empty_sequence
    >::type state_types_sequence;
  typedef typename boost::fusion::result_of::as_vector<state_types_sequence>::type state_types;

  state_types states_;
  bool viewport_attributes_applied_;
};

namespace detail
{

template<class ElementTag, class Context, SVGPP_TEMPLATE_ARGS>
class basic_shape_attribute_dispatcher:
  public attribute_dispatcher_base<ElementTag, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef attribute_dispatcher_base<ElementTag, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;
  typedef typename base_type::length_factory_type::length_type length_type;
  typedef typename boost::parameter::parameters<
    boost::parameter::optional<tag::basic_shapes_policy>
  >::bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::basic_shapes_policy, 
    typename policy::basic_shapes::by_context<Context>::type>::type basic_shapes_policy;
  typedef typename boost::mpl::if_< 
    boost::mpl::has_key<typename basic_shapes_policy::convert_to_path, ElementTag>,
    boost::mpl::single_view<
      typename boost::mpl::if_c<
        boost::is_same<ElementTag, tag::element::rect>::value 
          && basic_shapes_policy::convert_only_rounded_rect_to_path,
        convert_rounded_rect_to_path_state<length_type>, 
        convert_basic_shape_to_path_state<ElementTag, length_type>
      >::type
    >,
    typename boost::mpl::if_< 
      boost::mpl::has_key<typename basic_shapes_policy::collect_attributes, ElementTag>,
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

  bool on_exit_attributes()
  {
    on_exit_attributes_functor<Context, SVGPP_TEMPLATE_ARGS_PASS> fn(this->context_);
    return boost::fusion::accumulate(states_, true, fn);
  }

  using base_type::load_attribute_value; 

  template<class AttributeValue, class AttributeTag>
  typename boost::enable_if_c<
    !boost::mpl::empty<state_types_sequence>::value
    && boost::mpl::has_key<typename basic_shape_attributes<ElementTag>::type, AttributeTag>::value, bool>::type
  load_attribute_value(AttributeTag attribute_tag, 
                       AttributeValue const & attribute_value, 
                       tag::source::attribute property_source)
  {
    return 
      value_parser<
        typename traits::attribute_type<ElementTag, AttributeTag>::type, 
        SVGPP_TEMPLATE_ARGS_PASS
      >::parse(
        attribute_tag, 
        detail::adapt_context_value_events(this->context_, boost::fusion::at_c<0>(states_).get_own_context()), 
        attribute_value, property_source);
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
};

namespace detail
{

template<class ElementTag, class Context, SVGPP_TEMPLATE_ARGS>
class list_of_points_attribute_dispatcher:
  public attribute_dispatcher_base<ElementTag, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef attribute_dispatcher_base<ElementTag, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

  typedef typename boost::parameter::parameters<
    boost::parameter::optional<tag::basic_shapes_policy>
  >::bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::basic_shapes_policy, 
    typename policy::basic_shapes::by_context<Context>::type>::type basic_shapes_policy;

public:
  list_of_points_attribute_dispatcher(Context & context)
    : base_type(context)
  {
  }

  using base_type::load_attribute_value; 

  template<class AttributeValue>
  typename boost::enable_if_c<
    !boost::is_same<AttributeValue, void>::value // Just to make type dependent on template parameter
    && boost::mpl::has_key<typename basic_shapes_policy::convert_to_path, ElementTag>::value, bool>::type
  load_attribute_value(tag::attribute::points attribute_tag, AttributeValue const & attribute_value, 
                       tag::source::attribute property_source)
  {
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::path_policy>,
      boost::parameter::optional<tag::path_events_policy>,
      boost::parameter::optional<tag::markers_policy>,
      boost::parameter::optional<tag::marker_events_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef bind_context_parameters_wrapper<Context, args2_t> context_t;
    typedef path_adapter_if_needed<context_t> path_adapter_t; 
    typedef path_markers_adapter_if_needed<typename path_adapter_t::adapted_context> markers_adapter_t;

    context_t bound_context(this->context_);
    typename path_adapter_t::type path_adapter(detail::unwrap_context<context_t, tag::path_events_policy>::get(bound_context));
    typename path_adapter_t::adapted_context_holder adapted_path_context(path_adapter_t::adapt_context(bound_context, path_adapter));
    typename markers_adapter_t::type markers_adapter(adapted_path_context);

    return value_parser<traits::attribute_type<tag::element::polyline, tag::attribute::points>::type, 
        SVGPP_TEMPLATE_ARGS_PASS>::parse(
      attribute_tag, 
      adapt_context<tag::value_events_policy, list_of_points_to_path_adapter<ElementTag> >(adapted_path_context, markers_adapter_t::adapt_context(adapted_path_context, markers_adapter)),
      attribute_value, property_source);
  }
};

}

template<class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher<tag::element::polyline, Context, SVGPP_TEMPLATE_ARGS_PASS>:
  public detail::list_of_points_attribute_dispatcher<tag::element::polyline, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef detail::list_of_points_attribute_dispatcher<tag::element::polyline, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

public:
  attribute_dispatcher(Context & context)
    : base_type(context)
  {}
};

template<class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher<tag::element::polygon, Context, SVGPP_TEMPLATE_ARGS_PASS>:
  public detail::list_of_points_attribute_dispatcher<tag::element::polygon, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef detail::list_of_points_attribute_dispatcher<tag::element::polygon, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

public:
  attribute_dispatcher(Context & context)
    : base_type(context)
  {}
};

template<class Context, SVGPP_TEMPLATE_ARGS>
class attribute_dispatcher<tag::element::path, Context, SVGPP_TEMPLATE_ARGS_PASS>:
  public attribute_dispatcher_base<tag::element::path, Context, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef attribute_dispatcher_base<tag::element::path, Context, SVGPP_TEMPLATE_ARGS_PASS> base_type;

public:
  attribute_dispatcher(Context & context)
    : base_type(context)
  {}

  using base_type::load_attribute_value; 

  template<class AttributeValue>
  bool load_attribute_value(tag::attribute::d attribute_tag, AttributeValue const & attribute_value, 
                       tag::source::attribute property_source)
  {
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::path_policy>,
      boost::parameter::optional<tag::path_events_policy>,
      boost::parameter::optional<tag::markers_policy>,
      boost::parameter::optional<tag::marker_events_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef detail::bind_context_parameters_wrapper<Context, args2_t> context_t;
    typedef detail::path_adapter_if_needed<context_t> path_adapter_t; 
    typedef detail::path_markers_adapter_if_needed<typename path_adapter_t::adapted_context> markers_adapter_t;

    context_t bound_context(this->context_);
    typename path_adapter_t::type path_adapter(detail::unwrap_context<context_t, tag::path_events_policy>::get(bound_context));
    typename path_adapter_t::adapted_context_holder adapted_path_context(path_adapter_t::adapt_context(bound_context, path_adapter));
    typename markers_adapter_t::type markers_adapter(adapted_path_context);

    return value_parser<traits::attribute_type<tag::element::path, tag::attribute::d>::type, 
        SVGPP_TEMPLATE_ARGS_PASS>::parse(
      attribute_tag, 
      markers_adapter_t::adapt_context(adapted_path_context, markers_adapter),
      attribute_value, property_source);
  }
};

}
