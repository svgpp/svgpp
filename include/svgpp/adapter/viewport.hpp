// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/detail/adapt_context.hpp>
#include <svgpp/utility/calculate_viewbox_transform.hpp>
#include <boost/optional.hpp>
#include <boost/mpl/set.hpp>
#include <boost/noncopyable.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/variant.hpp>

namespace svgpp
{

// Pairs of <referencing element tag, referenced element tag> combinations in which 
// calculate_viewport_adapter needs to know type of referenced element
typedef boost::mpl::set3<
  boost::mpl::pair<tag::element::use_,  tag::element::svg>,
  boost::mpl::pair<tag::element::image, tag::element::svg>,
  boost::mpl::pair<tag::element::use_,  tag::element::symbol>
> viewport_adapter_needs_to_know_referencing_element;

namespace tag { namespace viewport_size_source
{
  struct use_own; // 'width' and 'height' attributes are set on element
  struct use_reference; // Like 'symbol' element referenced by 'use'
  struct reference_override_own; // Like 'svg' element referenced by 'use'
}}

template<
  class ReferencingElementTag, // 'void' means that element is not referenced or
                               // referencing element type doesn't matter (e.g. 'symbol' may be referenced only by 'use')
                               // see viewport_adapter_needs_to_know_referencing_element
  class ElementTag
>
struct get_viewport_size_source; // Shouldn't be called for anything other than 'svg' and 'symbol',
                                 // referenced by 'use' or 'image'
                                 // 'symbol' must only be used while referenced by 'use'

template<>
struct get_viewport_size_source<tag::element::use_, tag::element::svg>
{
  typedef tag::viewport_size_source::reference_override_own type;
};

template<>
struct get_viewport_size_source<tag::element::image, tag::element::svg>
{
  typedef tag::viewport_size_source::use_reference type;
};

template<>
struct get_viewport_size_source<void, tag::element::svg>
{
  typedef tag::viewport_size_source::use_own type;
};

template<>
struct get_viewport_size_source<tag::element::use_, tag::element::symbol>
{
  typedef tag::viewport_size_source::use_reference type;
};

template<>
struct get_viewport_size_source<void, tag::element::pattern>
{
  typedef tag::viewport_size_source::use_own type;
};

namespace detail
{
  template<class Length, class Coordinate, class ViewportSizeSourceTag>
  class calculate_viewport_adapter_size_holder;

  template<class Length, class Coordinate>
  class calculate_viewport_adapter_size_holder<Length, Coordinate, tag::viewport_size_source::use_own>
  {
  public:
    void set(tag::attribute::width,         Length const & val) { viewport_width_ = val; }
    void set(tag::attribute::height,        Length const & val) { viewport_height_ = val; }

  protected:
    template<class EventsPolicy, class Context, class LengthToUserCoordinatesConverter>
    void get_viewport_size(Context & context, LengthToUserCoordinatesConverter const & converter,
      Coordinate & viewport_width, Coordinate & viewport_height) const
    {
      viewport_width = converter.length_to_user_coordinate(
        viewport_width_
          ? *viewport_width_
          : converter.create_length(100, tag::length_units::percent(), tag::length_dimension::width()),
        tag::length_dimension::width());
      viewport_height = converter.length_to_user_coordinate(
        viewport_height_
          ? *viewport_height_
          : converter.create_length(100, tag::length_units::percent(), tag::length_dimension::height()),
        tag::length_dimension::height());
    }

  private:
    boost::optional<Length> viewport_width_, viewport_height_;
  };

  template<class Length, class Coordinate>
  class calculate_viewport_adapter_size_holder<Length, Coordinate, tag::viewport_size_source::use_reference>
  {
  protected:
    template<class EventsPolicy, class Context, class LengthToUserCoordinatesConverter>
    void get_viewport_size(Context & context, LengthToUserCoordinatesConverter const & converter,
      Coordinate & viewport_width, Coordinate & viewport_height) const
    {
      // Init with defaults first
      viewport_width = converter.length_to_user_coordinate(
        converter.create_length(100, tag::length_units::percent(), tag::length_dimension::width()),
        tag::length_dimension::width());
      viewport_height = converter.length_to_user_coordinate(
        converter.create_length(100, tag::length_units::percent(), tag::length_dimension::height()),
        tag::length_dimension::height());

      EventsPolicy::get_reference_viewport_size(context, viewport_width, viewport_height);
    }

    struct some_dummy_type;
    void set(some_dummy_type); // The only purpose of this declaration is to make
                               // "use base_type::set;" possible in inherited type
  };

  template<class Length, class Coordinate>
  class calculate_viewport_adapter_size_holder<Length, Coordinate, tag::viewport_size_source::reference_override_own>
    : public calculate_viewport_adapter_size_holder<Length, Coordinate, tag::viewport_size_source::use_own>
  {
  protected:
    template<class EventsPolicy, class Context, class LengthToUserCoordinatesConverter>
    void get_viewport_size(Context & context, LengthToUserCoordinatesConverter const & converter,
      Coordinate & viewport_width, Coordinate & viewport_height) const
    {
      calculate_viewport_adapter_size_holder<Length, Coordinate, tag::viewport_size_source::use_own>
        ::template get_viewport_size<EventsPolicy>(context, converter, viewport_width, viewport_height);
      // get_reference_viewport_size must only set values which was specified for referenced element
      // and keep values from current element if referenced element lacks them
      EventsPolicy::get_reference_viewport_size(context, viewport_width, viewport_height);
    }
  };

  template<class Coordinate>
  class collect_viewbox_adapter
  {
  protected:
    typedef boost::tuple<Coordinate, Coordinate, Coordinate, Coordinate> viewbox_type; // x, y, width, height

  public:
    collect_viewbox_adapter()
      : defer_(false)
      , align_(tag::value::xMidYMid())
      , meetOrSlice_(tag::value::meet())
    {}

    void set(tag::attribute::viewBox, Coordinate x, Coordinate y, Coordinate w, Coordinate h) 
    {
      viewbox_ = viewbox_type(x, y, w, h);
    }

    void set(tag::attribute::preserveAspectRatio, bool defer, tag::value::none align_tag)
    {
      defer_ = defer;
      align_ = align_tag;
    }

    template<class AlignTag, class MeetOrSliceTag>
    void set(tag::attribute::preserveAspectRatio, bool defer, AlignTag align_tag, MeetOrSliceTag meetOrSlice)
    {
      defer_ = defer;
      align_ = align_tag;
      meetOrSlice_ = meetOrSlice;
    }

  protected:
    boost::optional<viewbox_type> viewbox_;
    bool defer_;
    boost::variant<
      tag::value::none, 
      tag::value::xMinYMin,
      tag::value::xMidYMin,
      tag::value::xMaxYMin,
      tag::value::xMinYMid,
      tag::value::xMidYMid,
      tag::value::xMaxYMid,
      tag::value::xMinYMax,
      tag::value::xMidYMax,
      tag::value::xMaxYMax> align_; 
    boost::variant<tag::value::meet, tag::value::slice> meetOrSlice_; 

    template<class LengthToUserCoordinatesConverter>
    class options_visitor: 
      public boost::static_visitor<>,
      boost::noncopyable
    {
    public:
      options_visitor(viewbox_type const & viewbox,
        Coordinate viewport_width, Coordinate viewport_height,
        Coordinate & translate_x, Coordinate & translate_y, Coordinate & scale_x, Coordinate & scale_y)
        : viewbox_(viewbox)
        , viewport_width_(viewport_width)
        , viewport_height_(viewport_height)
        , translate_x_(translate_x)
        , translate_y_(translate_y)
        , scale_x_(scale_x)
        , scale_y_(scale_y)
      {
      }

      template<class AlignTag, class MeetOrSliceTag>
      void operator()(const AlignTag & align_tag, const MeetOrSliceTag & meetOrSlice_tag) const
      {
        calculate_viewbox_transform<Coordinate>::calculate(
          viewport_width_, viewport_height_,
          viewbox_.template get<0>(), viewbox_.template get<1>(), viewbox_.template get<2>(), viewbox_.template get<3>(),
          align_tag,
          meetOrSlice_tag,
          translate_x_, translate_y_, scale_x_, scale_y_);
      }

    private:
      viewbox_type const & viewbox_;
      Coordinate viewport_width_, viewport_height_;
      Coordinate & translate_x_, & translate_y_, & scale_x_, & scale_y_;
    };
  };

} // namespace detail

template<
  class ElementTag,
  class ReferencingElement, 
  class Length, 
  class Coordinate
>
class calculate_viewport_adapter: 
  public detail::calculate_viewport_adapter_size_holder<
    Length, 
    Coordinate, 
    typename get_viewport_size_source<ReferencingElement, ElementTag>::type
  >,
  public detail::collect_viewbox_adapter<Coordinate>,
  boost::noncopyable
{
  typedef detail::calculate_viewport_adapter_size_holder<
    Length, Coordinate, 
    typename get_viewport_size_source<ReferencingElement, ElementTag>::type
  > base_size_holder;
  typedef detail::collect_viewbox_adapter<Coordinate> base_viewbox_adapter;
  typedef boost::tuple<Coordinate, Coordinate, Coordinate, Coordinate> viewbox_type; // x, y, width, height

public:
  template<class Context>
  bool on_exit_attributes(Context & context) const
  {
    typedef detail::unwrap_context<Context, tag::viewport_events_policy> viewport_events;
    typedef detail::unwrap_context<Context, tag::error_policy> error_policy;
    typedef detail::unwrap_context<Context, tag::length_policy> length_policy_context;
    typedef typename length_policy_context::policy length_policy_t;

    typename length_policy_t::length_factory_type & converter 
      = length_policy_t::length_factory(length_policy_context::get(context));

    Coordinate viewport_x = converter.length_to_user_coordinate(
      viewport_x_ ? *viewport_x_ : converter.create_length(0, tag::length_units::none()),
      tag::length_dimension::width());
    Coordinate viewport_y = converter.length_to_user_coordinate(
      viewport_y_ ? *viewport_y_ : converter.create_length(0, tag::length_units::none()),
      tag::length_dimension::height());

    Coordinate viewport_width = 0, viewport_height = 0;
    base_size_holder::template get_viewport_size<typename viewport_events::policy>(context, converter, viewport_width, viewport_height);

    if (viewport_width == 0 || viewport_height == 0)
    {
      viewport_events::policy::disable_rendering(viewport_events::get(context));
      return true;
    }

    if (viewport_width < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::width());
    if (viewport_height < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::height());

    viewport_events::policy::set_viewport(viewport_events::get(context), viewport_x, viewport_y, viewport_width, viewport_height);

    if (this->viewbox_)
    {
      if (this->viewbox_->template get<2>() == 0 || this->viewbox_->template get<3>() == 0)
      {
        viewport_events::policy::disable_rendering(viewport_events::get(context));
        return true;
      }

      if (this->viewbox_->template get<2>() < 0 || this->viewbox_->template get<3>() < 0)
        return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::viewBox());

      Coordinate translate_x, translate_y, scale_x, scale_y;
      boost::apply_visitor(
        typename base_viewbox_adapter::template options_visitor<typename length_policy_t::length_factory_type>(*this->viewbox_,
          viewport_width, viewport_height,
          translate_x, translate_y, scale_x, scale_y), 
        this->align_, this->meetOrSlice_);
      viewport_events::policy::set_viewbox_transform(viewport_events::get(context), 
        translate_x, translate_y, scale_x, scale_y, this->defer_);

      viewport_events::policy::set_viewbox_size(viewport_events::get(context), 
        this->viewbox_->template get<2>(), this->viewbox_->template get<3>());
    }
    return true;
  }

  using base_size_holder::set;
  using base_viewbox_adapter::set;
  void set(tag::attribute::x,             Length const & val) { viewport_x_ = val; }
  void set(tag::attribute::y,             Length const & val) { viewport_y_ = val; }

private:
  boost::optional<Length> viewport_x_, viewport_y_;
};

struct viewport_transform_adapter
{
  template<class Context, class Coordinate>
  static void set_viewbox_transform(Context & context, Coordinate translate_x, Coordinate translate_y, 
    Coordinate scale_x, Coordinate scale_y, bool)
  {
    typedef detail::unwrap_context<Context, tag::transform_events_policy> transform_events;

    transform_events::policy::transform_translate(transform_events::get(context), translate_x, translate_y);
    transform_events::policy::transform_scale(transform_events::get(context), scale_x, scale_y);
  }

  template<class Context, class Coordinate>
  static void set_viewport(Context & context, Coordinate viewport_x, Coordinate viewport_y, 
    Coordinate viewport_width, Coordinate viewport_height)
  {
    typedef detail::unwrap_context<Context, tag::viewport_events_policy> viewport_events;
    typedef detail::unwrap_context<Context, tag::transform_events_policy> transform_events;

    viewport_events::policy::set_viewport(viewport_events::get(context), viewport_x, viewport_y, viewport_width, viewport_height);
    transform_events::policy::transform_translate(transform_events::get(context), viewport_x, viewport_y);
  }

  template<class Context, class Coordinate>
  static void get_reference_viewport_size(Context & context, Coordinate & viewport_width, Coordinate & viewport_height) 
  {
    typedef detail::unwrap_context<Context, tag::viewport_events_policy> viewport_events;

    viewport_events::policy::get_reference_viewport_size(viewport_events::get(context), viewport_width, viewport_height);
  }

  template<class Context, class Coordinate>
  static void set_viewbox_size(Context & context, Coordinate viewbox_width, Coordinate viewbox_height)
  {
    typedef detail::unwrap_context<Context, tag::viewport_events_policy> viewport_events;

    viewport_events::policy::set_viewbox_size(viewport_events::get(context), viewbox_width, viewbox_height);
  }

  template<class Context>
  static void disable_rendering(Context & context)
  {
    typedef detail::unwrap_context<Context, tag::viewport_events_policy> viewport_events;

    viewport_events::policy::disable_rendering(viewport_events::get(context));
  }
};

}