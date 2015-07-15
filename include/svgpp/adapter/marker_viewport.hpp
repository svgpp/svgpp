// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/adapter/viewport.hpp>

namespace svgpp
{

template<class Length, class Coordinate>
class calculate_marker_viewport_adapter: 
  public detail::collect_viewbox_adapter<Coordinate>,
  boost::noncopyable
{
  typedef detail::collect_viewbox_adapter<Coordinate> base_type;

public:
  template<class Context>
  bool on_exit_attributes(Context & context) const
  {
    typedef typename detail::unwrap_context<Context, tag::viewport_events_policy> viewport_events;
    typedef typename detail::unwrap_context<Context, tag::error_policy> error_policy;
    typedef typename detail::unwrap_context<Context, tag::length_policy> length_policy_context;
    typedef typename length_policy_context::policy length_policy_t;

    typename length_policy_t::length_factory_type & converter 
      = length_policy_t::length_factory(length_policy_context::get(context));

    Coordinate marker_width = converter.length_to_user_coordinate(
      marker_width_
        ? *marker_width_
        : converter.create_length(3, tag::length_units::none()),
      tag::length_dimension::width());
    Coordinate marker_height = converter.length_to_user_coordinate(
      marker_height_
        ? *marker_height_
        : converter.create_length(3, tag::length_units::none()),
      tag::length_dimension::height());

    if (marker_width == 0 || marker_height == 0)
    {
      viewport_events::policy::disable_rendering(viewport_events::get(context));
      return true;
    }

    if (marker_width < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::markerWidth());
    if (marker_height < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::markerHeight());

    if (this->viewbox_)
    {
      if (this->viewbox_->template get<2>() == 0 || this->viewbox_->template get<3>() == 0)
      {
        viewport_events::policy::disable_rendering(viewport_events::get(context));
        return true;
      }

      if (this->viewbox_->template get<2>() < 0 || this->viewbox_->template get<3>() < 0)
        return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::viewBox());

      viewport_events::policy::set_viewbox_size(viewport_events::get(context), 
        this->viewbox_->template get<2>(), this->viewbox_->template get<3>());
    }

    // Issue #34: refX and refY should be converted after set_viewbox_size is called,
    // because it may change viewport size and coefficients for length in percent
    Coordinate ref_x = converter.length_to_user_coordinate(
      ref_x_ ? *ref_x_ : converter.create_length(0, tag::length_units::none()),
      tag::length_dimension::width());
    Coordinate ref_y = converter.length_to_user_coordinate(
      ref_y_ ? *ref_y_ : converter.create_length(0, tag::length_units::none()),
      tag::length_dimension::height());

    if (this->viewbox_)
    {
      Coordinate translate_x, translate_y, scale_x, scale_y;
      boost::apply_visitor(
        typename base_type::template options_visitor<typename length_policy_t::length_factory_type>(*this->viewbox_,
          marker_width, marker_height,
          translate_x, translate_y, scale_x, scale_y), 
        this->align_, this->meetOrSlice_);
      Coordinate dx = -ref_x * scale_x - translate_x;
      Coordinate dy = -ref_y * scale_y - translate_y;
      viewport_events::policy::set_viewport(viewport_events::get(context), dx, dy, marker_width, marker_height);
      viewport_events::policy::set_viewbox_transform(viewport_events::get(context), translate_x, translate_y, scale_x, scale_y, this->defer_);
    }
    else
      viewport_events::policy::set_viewport(viewport_events::get(context), -ref_x, -ref_y, marker_width, marker_height);
    return true;
  }

  using detail::collect_viewbox_adapter<Coordinate>::set;

  void set(tag::attribute::refX,    Length const & val) { ref_x_ = val; }
  void set(tag::attribute::refY,    Length const & val) { ref_y_ = val; }
  void set(tag::attribute::markerWidth,   Length const & val) { marker_width_ = val; }
  void set(tag::attribute::markerHeight,  Length const & val) { marker_height_ = val; }

private:
  boost::optional<Length> ref_x_, ref_y_;
  boost::optional<Length> marker_width_, marker_height_;
};

}