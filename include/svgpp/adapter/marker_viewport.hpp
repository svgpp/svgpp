#pragma once

#include <svgpp/adapter/viewport.hpp>

namespace svgpp
{

template<class Length, class Coordinate>
class calculate_marker_viewport_adapter: 
  public detail::collect_viewbox_adapter<Coordinate>,
  boost::noncopyable
{
public:
  template<class Context>
  bool on_exit_attributes(Context & context) const
  {
    typedef typename detail::unwrap_context<Context, tag::load_value_policy> load_value;
    typedef typename detail::unwrap_context<Context, tag::error_policy> error_policy;
    typedef typename detail::unwrap_context<Context, tag::length_policy> length_policy_context;
    typedef typename length_policy_context::policy length_policy_t;

    typename length_policy_t::length_factory_type & converter 
      = length_policy_t::length_factory(length_policy_context::get(context));

    Coordinate ref_x = converter.length_to_user_coordinate(
      ref_x_ ? *ref_x_ : converter.create_length(0, tag::length_units::none()),
      tag::width_length());
    Coordinate ref_y = converter.length_to_user_coordinate(
      ref_y_ ? *ref_y_ : converter.create_length(0, tag::length_units::none()),
      tag::height_length());

    Coordinate marker_width = converter.length_to_user_coordinate(
      marker_width_
        ? *marker_width_
        : converter.create_length(3, tag::length_units::none()),
      tag::width_length());
    Coordinate marker_height = converter.length_to_user_coordinate(
      marker_height_
        ? *marker_height_
        : converter.create_length(3, tag::length_units::none()),
      tag::height_length());

    if (marker_width == 0 || marker_height == 0)
      // TODO: disable rendering
      return true;

    if (marker_width < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::markerWidth());
    if (marker_height < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::markerHeight());

    if (viewbox_)
    {
      if (viewbox_->get<2>() == 0 || viewbox_->get<3>() == 0)
        // TODO: disable rendering
        return true;
      if (viewbox_->get<2>() < 0 || viewbox_->get<3>() < 0)
        return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::viewBox());

      Coordinate translate_x, translate_y, scale_x, scale_y;
      boost::apply_visitor(
        options_visitor<length_policy_t::length_factory_type>(*viewbox_,
          marker_width, marker_height,
          translate_x, translate_y, scale_x, scale_y), 
        align_, meetOrSlice_);
      Coordinate dx = -ref_x * scale_x - translate_x;
      Coordinate dy = -ref_y * scale_y - translate_y;
      load_value::policy::set_viewport(load_value::get(context), dx, dy, marker_width, marker_height);
      load_value::policy::set_viewbox_transform(load_value::get(context), translate_x, translate_y, scale_x, scale_y, defer_);
    }
    else
      load_value::policy::set_viewport(load_value::get(context), -ref_x, -ref_y, marker_width, marker_height);
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