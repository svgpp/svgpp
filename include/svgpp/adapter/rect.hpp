// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/adapter/path.hpp>
#include <svgpp/detail/adapt_context.hpp>
#include <boost/mpl/set.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

namespace svgpp
{

typedef boost::mpl::set6<
  tag::attribute::x, 
  tag::attribute::y, 
  tag::attribute::rx, 
  tag::attribute::ry,
  tag::attribute::width, 
  tag::attribute::height
> rect_shape_attributes;

template<class Length>
class collect_rect_attributes_adapter: boost::noncopyable
{
public:
  template<class Context>
  bool on_exit_attributes(Context & context) const
  {
    typedef detail::unwrap_context<Context, tag::length_policy> length_policy_context;
    typedef typename length_policy_context::policy length_policy_t;
    typedef detail::unwrap_context<Context, tag::error_policy> error_policy;

    typename length_policy_t::length_factory_type & converter 
      = length_policy_t::length_factory(length_policy_context::get(context));

    typename length_policy_t::length_factory_type::coordinate_type
      x = 0, y = 0, rx = 0, ry = 0, width, height;
    if (x_)
      x = converter.length_to_user_coordinate(*x_, tag::length_dimension::width());
    if (y_)
      y = converter.length_to_user_coordinate(*y_, tag::length_dimension::height());
    width = converter.length_to_user_coordinate(width_, tag::length_dimension::width());
    height = converter.length_to_user_coordinate(height_, tag::length_dimension::height());
    if (rx_)
    {
      rx = converter.length_to_user_coordinate(*rx_, tag::length_dimension::width());
      if (!ry_)
        ry = rx;
    }
    if (ry_)
    {
      ry = converter.length_to_user_coordinate(*ry_, tag::length_dimension::height());
      if (!rx_)
        rx = ry;
    }
    if (rx < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::rx());
    if (ry < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::ry());
    if (width < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::width());
    if (height < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::height());

    if (width == 0 || height == 0)
      return true;

    typedef detail::unwrap_context<Context, tag::basic_shapes_events_policy> basic_shapes_events;
    basic_shapes_events::policy::set_rect(basic_shapes_events::get(context), x, y, width, height, rx, ry);
    return true;
  }

  void set(tag::attribute::x, Length const & val) { x_ = val; }
  void set(tag::attribute::y, Length const & val) { y_ = val; }
  void set(tag::attribute::rx, Length const & val) { rx_ = val; }
  void set(tag::attribute::ry, Length const & val) { ry_ = val; }
  void set(tag::attribute::width, Length const & val) { width_ = val; }
  void set(tag::attribute::height, Length const & val) { height_ = val; }

private:
  boost::optional<Length> x_, y_, rx_, ry_;
  Length width_, height_; // Required attributes
};

namespace detail
{
  template<class EventsPolicy, class Context, class Coordinate>
  inline void context_set_rounded_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height,
    Coordinate rx, Coordinate ry)
  {
    rx = std::min(rx, width / 2);
    ry = std::min(ry, height / 2);
    EventsPolicy::path_move_to(context, x + rx, y, tag::coordinate::absolute());
    EventsPolicy::path_line_to_ortho(context, x+width-rx, true, tag::coordinate::absolute());
    EventsPolicy::path_elliptical_arc_to(context, rx, ry, Coordinate(0), false, true, x+width, y+ry, tag::coordinate::absolute()); 
    EventsPolicy::path_line_to_ortho(context, y+height-ry, false, tag::coordinate::absolute());
    EventsPolicy::path_elliptical_arc_to(context, rx, ry, Coordinate(0), false, true, x+width-rx, y+height, tag::coordinate::absolute()); 
    EventsPolicy::path_line_to_ortho(context, x+rx, true, tag::coordinate::absolute());
    EventsPolicy::path_elliptical_arc_to(context, rx, ry, Coordinate(0), false, true, x, y+height-ry, tag::coordinate::absolute()); 
    EventsPolicy::path_line_to_ortho(context, y+ry, false, tag::coordinate::absolute());
    EventsPolicy::path_elliptical_arc_to(context, rx, ry, Coordinate(0), false, true, x+rx, y, tag::coordinate::absolute()); 
    EventsPolicy::path_close_subpath(context);
    EventsPolicy::path_exit(context);
  }
}

struct rect_to_path_adapter
{
  template<class Context, class Coordinate>
  static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height,
    Coordinate rx, Coordinate ry)
  {
    typedef detail::unwrap_context<Context, tag::path_events_policy> path_events;

    typename path_events::type & path_context = path_events::get(context);
    if (rx == 0 || ry == 0)
    {
      path_events::policy::path_move_to(path_context, x, y, tag::coordinate::absolute());
      path_events::policy::path_line_to_ortho(path_context, width,  true,  tag::coordinate::relative());
      path_events::policy::path_line_to_ortho(path_context, height, false, tag::coordinate::relative());
      path_events::policy::path_line_to_ortho(path_context, -width, true,  tag::coordinate::relative());
      path_events::policy::path_close_subpath(path_context);
      path_events::policy::path_exit(path_context);
    }
    else
    {
      detail::context_set_rounded_rect<typename path_events::policy>(path_context, x, y, width, height, rx, ry);
    }
  }
};

struct rounded_rect_to_path_adapter
{
  template<class Context, class Coordinate>
  static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height,
    Coordinate rx, Coordinate ry)
  {
    if (rx == 0 || ry == 0)
    {
      typedef typename detail::unwrap_context<Context, tag::basic_shapes_events_policy> basic_shapes_events;
      basic_shapes_events::policy::set_rect(basic_shapes_events::get(context), x, y, width, height);
    }
    else
    {
      typedef typename detail::unwrap_context<Context, tag::path_events_policy> path_events;
      detail::context_set_rounded_rect<typename path_events::policy>(path_events::get(context), x, y, width, height, rx, ry);
    }
  }
};

}