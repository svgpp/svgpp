// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/noncopyable.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/static_assert.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <svgpp/policy/path.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/detail/adapt_context.hpp>
#include <svgpp/policy/path_events.hpp>
#include <svgpp/utility/arc_endpoint_to_center.hpp>
#include <svgpp/utility/arc_to_bezier.hpp>

namespace svgpp
{

// TODO: option to request arc by bezier estimation error from user code (now it is set by maximum angle step)

namespace detail
{
  template<class Coordinate>
  struct path_adapter_cubic_bezier_data
  {
    typedef Coordinate coordinate_type;

    coordinate_type last_cubic_bezier_cp_x, last_cubic_bezier_cp_y;
    bool last_cubic_bezier_cp_valid;

    path_adapter_cubic_bezier_data()
      : last_cubic_bezier_cp_valid(false)
    {
    }

    void set_cubic_cp_base(coordinate_type absolute_x, coordinate_type absolute_y)
    {
      last_cubic_bezier_cp_x = absolute_x;
      last_cubic_bezier_cp_y = absolute_y;
      last_cubic_bezier_cp_valid = true;
    }

    void invalidate_last_cubic_bezier_cp()
    {
      last_cubic_bezier_cp_valid = false;
    }
  };

  template<class Coordinate>
  struct path_adapter_cubic_bezier_stub
  {
    typedef Coordinate coordinate_type;

    void set_cubic_cp_base(coordinate_type, coordinate_type)
    {
    }

    void invalidate_last_cubic_bezier_cp()
    {
    }
  };

  template<class Coordinate>
  struct path_adapter_quadratic_bezier_data
  {
    typedef Coordinate coordinate_type;

    coordinate_type last_quadratic_bezier_cp_x, last_quadratic_bezier_cp_y;
    bool last_quadratic_bezier_cp_valid;

    path_adapter_quadratic_bezier_data()
      : last_quadratic_bezier_cp_valid(false)
    {
    }

    void set_quadratic_cp_base(coordinate_type absolute_x, coordinate_type absolute_y)
    {
      last_quadratic_bezier_cp_x = absolute_x;
      last_quadratic_bezier_cp_y = absolute_y;
      last_quadratic_bezier_cp_valid = true;
    }

    void invalidate_last_quadratic_bezier_cp()
    {
      last_quadratic_bezier_cp_valid = false;
    }
  };

  template<class Coordinate>
  struct path_adapter_quadratic_bezier_stub
  {
    typedef Coordinate coordinate_type;

    void set_quadratic_cp_base(coordinate_type, coordinate_type)
    {
    }

    void invalidate_last_quadratic_bezier_cp()
    {
    }
  };
}

template<
  class OutputContext, 
  class PathPolicy = typename policy::path::by_context<OutputContext>::type, 
  class Coordinate = typename number_type_by_context<OutputContext>::type,
  class EventsPolicy = policy::path_events::default_policy<OutputContext> >
class path_adapter: 
  boost::noncopyable,
  private boost::mpl::if_c<PathPolicy::no_cubic_bezier_shorthand, 
    detail::path_adapter_cubic_bezier_data<Coordinate>,
    detail::path_adapter_cubic_bezier_stub<Coordinate>
  >::type,
  private boost::mpl::if_c<PathPolicy::no_quadratic_bezier_shorthand || PathPolicy::quadratic_bezier_as_cubic, 
    detail::path_adapter_quadratic_bezier_data<Coordinate>,
    detail::path_adapter_quadratic_bezier_stub<Coordinate>
  >::type
{
public:
  typedef Coordinate coordinate_type;

  explicit path_adapter(OutputContext & original_context_)
    : output_context(original_context_)
    , current_x(0), current_y(0)
    , subpath_start_x(0), subpath_start_y(0)
  {
    BOOST_STATIC_ASSERT_MSG(!PathPolicy::quadratic_bezier_as_cubic || PathPolicy::no_cubic_bezier_shorthand, "First option requires second one");
  }

private:
  void non_curve_command()
  {
    this->invalidate_last_cubic_bezier_cp();
    this->invalidate_last_quadratic_bezier_cp();
  }

  void set_cubic_cp(coordinate_type absolute_x, coordinate_type absolute_y)
  {
    this->set_cubic_cp_base(absolute_x, absolute_y);
    this->invalidate_last_quadratic_bezier_cp();
  }

  void set_quadratic_cp(coordinate_type absolute_x, coordinate_type absolute_y)
  {
    this->set_quadratic_cp_base(absolute_x, absolute_y);
    this->invalidate_last_cubic_bezier_cp();
  }

  OutputContext & output_context;
  coordinate_type current_x, current_y, subpath_start_x, subpath_start_y;

public:
  OutputContext & get_output_context() const { return output_context; }

  template<class Policy>
  void path_move_to(
    coordinate_type x, 
    coordinate_type y, tag::coordinate::absolute tag)
  { 
    EventsPolicy::path_move_to(output_context, x, y, tag);
    non_curve_command();
    current_x = x;
    current_y = y;
    subpath_start_x = x;
    subpath_start_y = y;
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::absolute_coordinates_only>::type
  path_move_to(
    coordinate_type x, 
    coordinate_type y, tag::coordinate::relative)
  { 
    current_x += x;
    current_y += y;
    path_move_to<Policy>(current_x, current_y, tag::coordinate::absolute());
    non_curve_command();
    subpath_start_x = current_x;
    subpath_start_y = current_y;
  }

  template<class Policy>
  typename boost::disable_if_c<Policy::absolute_coordinates_only>::type
  path_move_to(
    coordinate_type x, 
    coordinate_type y, tag::coordinate::relative tag)
  { 
    current_x += x;
    current_y += y;
    EventsPolicy::path_move_to(output_context, x, y, tag);
    non_curve_command();
    subpath_start_x = current_x;
    subpath_start_y = current_y;
  }

  template<class Policy>
  void path_line_to(
    coordinate_type x, 
    coordinate_type y, tag::coordinate::absolute tag)
  { 
    current_x = x;
    current_y = y;
    EventsPolicy::path_line_to(output_context, x, y, tag);
    non_curve_command();
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::absolute_coordinates_only>::type
  path_line_to(
    coordinate_type x, 
    coordinate_type y, tag::coordinate::relative)
  {
    current_x += x;
    current_y += y;
    EventsPolicy::path_line_to(output_context, current_x, current_y, tag::coordinate::absolute());
    non_curve_command();
  }

  template<class Policy>
  typename boost::disable_if_c<Policy::absolute_coordinates_only>::type
  path_line_to(
    coordinate_type x, 
    coordinate_type y, tag::coordinate::relative tag)
  {
    current_x += x;
    current_y += y;
    EventsPolicy::path_line_to(output_context, x, y, tag);
    non_curve_command();
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::no_ortho_line_to>::type
  path_line_to_ortho(
    coordinate_type coord, 
    bool horizontal, tag::coordinate::relative tag)
  {
    if (horizontal)
      path_line_to<Policy>(coord, 0, tag);
    else
      path_line_to<Policy>(0, coord, tag);
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::no_ortho_line_to>::type
  path_line_to_ortho(
    coordinate_type coord, 
    bool horizontal, tag::coordinate::absolute tag)
  {
    if (horizontal)
      path_line_to<Policy>(coord, current_y, tag);
    else
      path_line_to<Policy>(current_x, coord, tag);
  }

  template<class Policy>
  typename boost::disable_if_c<Policy::no_ortho_line_to>::type
  path_line_to_ortho(
    coordinate_type coord, bool horizontal, tag::coordinate::absolute tag)
  {
    if (horizontal)
      current_x = coord;
    else
      current_y = coord;
    EventsPolicy::path_line_to_ortho(output_context, coord, horizontal, tag);
    non_curve_command();
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::absolute_coordinates_only && !Policy::no_ortho_line_to>::type
  path_line_to_ortho(
    coordinate_type coord, bool horizontal, tag::coordinate::relative tag)
  {
    if (horizontal)
      current_x += coord;
    else
      current_y += coord;
    EventsPolicy::path_line_to_ortho(output_context, 
      horizontal ? current_x : current_y, horizontal, tag::coordinate::absolute());
    non_curve_command();
  }

  template<class Policy>
  typename boost::enable_if_c<!Policy::absolute_coordinates_only && !Policy::no_ortho_line_to>::type
  path_line_to_ortho(
    coordinate_type coord, bool horizontal, tag::coordinate::relative tag)
  {
    if (horizontal)
      current_x += coord;
    else
      current_y += coord;
    EventsPolicy::path_line_to_ortho(output_context, coord, horizontal, tag);
    non_curve_command();
  }

  template<class Policy>
  typename boost::disable_if_c<Policy::quadratic_bezier_as_cubic>::type
  path_quadratic_bezier_to(
    coordinate_type x1, 
    coordinate_type y1, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::absolute tag)
  {
    current_x = x;
    current_y = y;
    EventsPolicy::path_quadratic_bezier_to(output_context, x1, y1, x, y, tag);
    set_quadratic_cp(x1, y1);
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::absolute_coordinates_only && !Policy::quadratic_bezier_as_cubic>::type
  path_quadratic_bezier_to(
    coordinate_type x1, 
    coordinate_type y1, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::relative)
  {
    x += current_x;
    y += current_y;
    x1 += current_x;
    y1 += current_y;
    current_x = x;
    current_y = y;
    EventsPolicy::path_quadratic_bezier_to(output_context, x1, y1, x, y, tag::coordinate::absolute());
    set_quadratic_cp(x1, y1);
  }

  template<class Policy>
  typename boost::enable_if_c<!Policy::absolute_coordinates_only && !Policy::quadratic_bezier_as_cubic>::type
  path_quadratic_bezier_to(
    coordinate_type x1, 
    coordinate_type y1, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::relative tag)
  {
    EventsPolicy::path_quadratic_bezier_to(output_context, x1, y1, x, y, tag);
    set_quadratic_cp(x1 + current_x, y1 + current_y);
    current_x += x;
    current_y += y;
  }

  template<class Policy>
  typename boost::enable_if_c<
                              Policy::no_quadratic_bezier_shorthand
                              || Policy::quadratic_bezier_as_cubic>::type
  path_quadratic_bezier_to(
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::absolute tag)
  {
    if (this->last_quadratic_bezier_cp_valid)
      path_quadratic_bezier_to<Policy>(
        2 * current_x - this->last_quadratic_bezier_cp_x,
        2 * current_y - this->last_quadratic_bezier_cp_y, 
        x, y, tag);
    else
      path_quadratic_bezier_to<Policy>(current_x, current_y, x, y, tag);
  }

  template<class Policy>
  typename boost::enable_if_c<
                              Policy::no_quadratic_bezier_shorthand
                              || Policy::quadratic_bezier_as_cubic>::type
  path_quadratic_bezier_to(
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::relative tag)
  {
    if (this->last_quadratic_bezier_cp_valid)
      path_quadratic_bezier_to<Policy>(
        current_x - this->last_quadratic_bezier_cp_x,
        current_y - this->last_quadratic_bezier_cp_y, x, y, tag);
    else
      path_quadratic_bezier_to<Policy>(0, 0, x, y, tag);
  }

  template<class Policy>
  typename boost::disable_if_c<
                                Policy::no_quadratic_bezier_shorthand
                                || Policy::quadratic_bezier_as_cubic>::type
  path_quadratic_bezier_to(
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::absolute tag)
  {
    EventsPolicy::path_quadratic_bezier_to(output_context, x, y, tag);
    if (this->last_quadratic_bezier_cp_valid)
      set_quadratic_cp(
        2 * current_x - this->last_quadratic_bezier_cp_x,
        2 * current_y - this->last_quadratic_bezier_cp_y);
    else
      set_quadratic_cp(current_x, current_y);
    current_x = x;
    current_y = y;
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::absolute_coordinates_only
    && !(Policy::no_quadratic_bezier_shorthand
      || Policy::quadratic_bezier_as_cubic)>::type
  path_quadratic_bezier_to(
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::relative tag)
  {
    x += current_x;
    y += current_y;
    EventsPolicy::path_quadratic_bezier_to(output_context, x, y, tag::coordinate::absolute());
    if (this->last_quadratic_bezier_cp_valid)
      set_quadratic_cp(
        2 * current_x - this->last_quadratic_bezier_cp_x,
        2 * current_y - this->last_quadratic_bezier_cp_y);
    else
      set_quadratic_cp(current_x, current_y);
    current_x = x;
    current_y = y;
  }

  template<class Policy>
  typename boost::enable_if_c<!Policy::absolute_coordinates_only
    && !(Policy::no_quadratic_bezier_shorthand
    || Policy::quadratic_bezier_as_cubic)>::type
  path_quadratic_bezier_to(
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::relative tag)
  {
    EventsPolicy::path_quadratic_bezier_to(output_context, x, y, tag);
    if (this->last_quadratic_bezier_cp_valid)
      set_quadratic_cp(
        2 * current_x - this->last_quadratic_bezier_cp_x,
        2 * current_y - this->last_quadratic_bezier_cp_y);
    else
      set_quadratic_cp(current_x, current_y);
    current_x += x;
    current_y += y;
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::quadratic_bezier_as_cubic>::type
  path_quadratic_bezier_to(
    coordinate_type x1, 
    coordinate_type y1, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::absolute tag)
  {
    BOOST_STATIC_ASSERT(boost::is_floating_point<coordinate_type>::value);
    const coordinate_type k1 = 1./3.;
    const coordinate_type k2 = 2./3.;
    coordinate_type xk = k2 * x1;
    coordinate_type yk = k2 * y1;
    path_cubic_bezier_to<Policy>(
      current_x * k1 + xk, current_y * k1 + yk,
      x * k1 + xk, y * k1 + yk,
      x, y, tag, false);
    set_quadratic_cp(x1, y1);
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::quadratic_bezier_as_cubic>::type
  path_quadratic_bezier_to(
    coordinate_type x1, 
    coordinate_type y1, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::relative tag)
  {
    BOOST_STATIC_ASSERT(boost::is_floating_point<coordinate_type>::value);
    const coordinate_type k1 = 1./3.;
    const coordinate_type k2 = 2./3.;
    coordinate_type xk = k2 * x1;
    coordinate_type yk = k2 * y1;
    const coordinate_type x1_absolute = x1 + current_x;
    const coordinate_type y1_absolute = y1 + current_y;
    path_cubic_bezier_to<Policy>(
      xk, yk,
      x * k1 + xk, y * k1 + yk,
      x, y, tag, false);
    set_quadratic_cp(x1_absolute, y1_absolute);
  }

  template<class Policy>
  void path_cubic_bezier_to(
    coordinate_type x1, 
    coordinate_type y1, 
    coordinate_type x2, 
    coordinate_type y2, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::absolute tag,
    bool do_set_cubic_cp = true)
  {
    current_x = x;
    current_y = y;
    EventsPolicy::path_cubic_bezier_to(output_context, x1, y1, x2, y2, x, y, tag);
    if (do_set_cubic_cp)
      set_cubic_cp(x2, y2);
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::absolute_coordinates_only>::type
  path_cubic_bezier_to(
    coordinate_type x1, 
    coordinate_type y1, 
    coordinate_type x2, 
    coordinate_type y2, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::relative,
    bool do_set_cubic_cp = true)
  {
    x += current_x;
    y += current_y;
    x1 += current_x;
    y1 += current_y;
    x2 += current_x;
    y2 += current_y;
    EventsPolicy::path_cubic_bezier_to(output_context, x1, y1, x2, y2, x, y, tag::coordinate::absolute());
    if (do_set_cubic_cp)
      set_cubic_cp(x2, y2);
    current_x = x;
    current_y = y;
  }

  template<class Policy>
  typename boost::disable_if_c<Policy::absolute_coordinates_only>::type
  path_cubic_bezier_to(
    coordinate_type x1, 
    coordinate_type y1, 
    coordinate_type x2, 
    coordinate_type y2, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::relative tag,
    bool do_set_cubic_cp = true)
  {
    EventsPolicy::path_cubic_bezier_to(output_context, x1, y1, x2, y2, x, y, tag);
    if (do_set_cubic_cp)
      set_cubic_cp(x2 + current_x, y2 + current_y);
    current_x += x;
    current_y += y;
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::no_cubic_bezier_shorthand>::type
  path_cubic_bezier_to(
      coordinate_type x2, 
      coordinate_type y2, 
      coordinate_type x, 
      coordinate_type y, 
      tag::coordinate::absolute tag)
  {
    if (this->last_cubic_bezier_cp_valid)
      path_cubic_bezier_to<Policy>( 
        2 * current_x - this->last_cubic_bezier_cp_x,
        2 * current_y - this->last_cubic_bezier_cp_y, 
        x2, y2, x, y, tag);
    else
      path_cubic_bezier_to<Policy>(current_x, current_y, x2, y2, x, y, tag);
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::no_cubic_bezier_shorthand>::type
  path_cubic_bezier_to(
    coordinate_type x2, 
    coordinate_type y2, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::relative tag)
  {
    if (this->last_cubic_bezier_cp_valid)
      path_cubic_bezier_to<Policy>(
        current_x - this->last_cubic_bezier_cp_x,
        current_y - this->last_cubic_bezier_cp_y, x2, y2, x, y, tag);
    else
      path_cubic_bezier_to<Policy>(0, 0, x2, y2, x, y, tag);
  }

  template<class Policy>
  typename boost::disable_if_c<Policy::no_cubic_bezier_shorthand>::type
  path_cubic_bezier_to(
    coordinate_type x2, 
    coordinate_type y2, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::absolute tag)
  {
    current_x = x;
    current_y = y;
    EventsPolicy::path_cubic_bezier_to(output_context, x2, y2, x, y, tag);
    set_cubic_cp(x2, y2);
  }

  template<class Policy>
  typename boost::disable_if_c<Policy::no_cubic_bezier_shorthand>::type
  path_cubic_bezier_to(
    coordinate_type x2, 
    coordinate_type y2, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::relative tag)
  {
    EventsPolicy::path_cubic_bezier_to(output_context, x2, y2, x, y, tag);
    set_cubic_cp(current_x + x2, current_y + y2);
    current_x += x;
    current_y += y;
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::arc_as_cubic_bezier>::type
  path_elliptical_arc_to(
    coordinate_type rx, 
    coordinate_type ry, 
    coordinate_type x_axis_rotation, 
    bool large_arc_flag, bool sweep_flag, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::absolute tag)
  { 
    x_axis_rotation *= boost::math::constants::degree<coordinate_type>();
    coordinate_type cx, cy, theta1, theta2;
    arc_endpoint_to_center(current_x, current_y, x, y,
      rx, ry, x_axis_rotation, large_arc_flag, sweep_flag,
      cx, cy, theta1, theta2);
    if (sweep_flag)
    {
      if (theta2 < theta1)
        theta2 += boost::math::constants::two_pi<coordinate_type>();
    }
    else
    {
      if (theta2 > theta1)
        theta2 -= boost::math::constants::two_pi<coordinate_type>();
    }

    typedef arc_to_bezier<coordinate_type> arc_to_bezier_t;
    arc_to_bezier_t a2b(cx, cy, rx, ry, x_axis_rotation, 
      typename arc_to_bezier_t::circle_angle_tag(), theta1, theta2, 
      typename arc_to_bezier_t::max_angle_tag(), boost::math::constants::half_pi<coordinate_type>());
    for(typename arc_to_bezier_t::iterator it(a2b); !it.eof(); it.advance())
      path_cubic_bezier_to<Policy>(
        it.p1x(), it.p1y(),
        it.p2x(), it.p2y(),
        it.p3x(), it.p3y(),
        tag::coordinate::absolute(),
        false);
  } 

  template<class Policy>
  typename boost::disable_if_c<Policy::arc_as_cubic_bezier>::type
  path_elliptical_arc_to(
    coordinate_type rx, 
    coordinate_type ry, 
    coordinate_type x_axis_rotation, 
    bool large_arc_flag, bool sweep_flag, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::absolute tag)
  { 
    EventsPolicy::path_elliptical_arc_to(output_context, rx, ry, x_axis_rotation, large_arc_flag, sweep_flag, x, y, tag); 
    non_curve_command();
    current_x = x;
    current_y = y;
  }

  template<class Policy>
  typename boost::enable_if_c<Policy::absolute_coordinates_only || Policy::arc_as_cubic_bezier>::type
  path_elliptical_arc_to(
    coordinate_type rx, 
    coordinate_type ry, 
    coordinate_type x_axis_rotation, 
    bool large_arc_flag, bool sweep_flag, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::relative)
  { 
    path_elliptical_arc_to<Policy>( 
      rx, ry, x_axis_rotation, large_arc_flag, sweep_flag, x + current_x, y + current_y, tag::coordinate::absolute()); 
  }

  template<class Policy>
  typename boost::disable_if_c<Policy::absolute_coordinates_only || Policy::arc_as_cubic_bezier>::type
  path_elliptical_arc_to(
    coordinate_type rx, 
    coordinate_type ry, 
    coordinate_type x_axis_rotation, 
    bool large_arc_flag, bool sweep_flag, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::relative tag)
  { 
    EventsPolicy::path_elliptical_arc_to(output_context, 
      rx, ry, x_axis_rotation, large_arc_flag, sweep_flag, x, y, tag); 
    non_curve_command();
    current_x += x;
    current_y += y;
  }

  template<class Policy>
  void path_close_subpath()
  {
    EventsPolicy::path_close_subpath(output_context);
    non_curve_command();
    current_x = subpath_start_x;
    current_y = subpath_start_y;
  }

  template<class Policy>
  void path_exit()
  {
    EventsPolicy::path_exit(output_context);
  }
};

namespace detail
{

// This class created just to pass PathPolicy as an template argument to
// path_adapter methods so that boost::enable_if can be used
template<
  class Adapter, 
  class PathPolicy, 
  class Coordinate
>
struct path_adapter_path_events_policy
{
  typedef Adapter context_type;

  template<class AbsoluteOrRelative>
  static void path_move_to(Adapter & context, Coordinate x, Coordinate y, AbsoluteOrRelative absoluteOrRelative)
  { 
    context.template path_move_to<PathPolicy>(x, y, absoluteOrRelative);
  }

  template<class AbsoluteOrRelative>
  static void path_line_to(Adapter & context, Coordinate x, Coordinate y, AbsoluteOrRelative absoluteOrRelative)
  { 
    context.template path_line_to<PathPolicy>(x, y, absoluteOrRelative);
  }

  template<class AbsoluteOrRelative>
  static void path_line_to_ortho(Adapter & context, Coordinate coord, bool horizontal, AbsoluteOrRelative absoluteOrRelative)
  { 
    context.template path_line_to_ortho<PathPolicy>(coord, horizontal, absoluteOrRelative);
  }

  template<class AbsoluteOrRelative>
  static void path_cubic_bezier_to(Adapter & context, Coordinate x1, Coordinate y1, 
    Coordinate x2, Coordinate y2, 
    Coordinate x, Coordinate y, 
    AbsoluteOrRelative absoluteOrRelative)
  { 
    context.template path_cubic_bezier_to<PathPolicy>(x1, y1, x2, y2, x, y, absoluteOrRelative);
  }

  template<class AbsoluteOrRelative>
  static void path_cubic_bezier_to(Adapter & context, 
    Coordinate x2, Coordinate y2, 
    Coordinate x, Coordinate y, 
    AbsoluteOrRelative absoluteOrRelative)
  { 
    context.template path_cubic_bezier_to<PathPolicy>(x2, y2, x, y, absoluteOrRelative);
  }

  template<class AbsoluteOrRelative>
  static void path_quadratic_bezier_to(Adapter & context, 
    Coordinate x1, Coordinate y1, 
    Coordinate x, Coordinate y, 
    AbsoluteOrRelative absoluteOrRelative)
  { 
    context.template path_quadratic_bezier_to<PathPolicy>(x1, y1, x, y, absoluteOrRelative);
  }

  template<class AbsoluteOrRelative>
  static void path_quadratic_bezier_to(Adapter & context, 
    Coordinate x, Coordinate y, 
    AbsoluteOrRelative absoluteOrRelative)
  { 
    context.template path_quadratic_bezier_to<PathPolicy>(x, y, absoluteOrRelative);
  }

  template<class AbsoluteOrRelative>
  static void path_elliptical_arc_to(Adapter & context, 
    Coordinate rx, Coordinate ry, Coordinate x_axis_rotation,
    bool large_arc_flag, bool sweep_flag, 
    Coordinate x, Coordinate y,
    AbsoluteOrRelative absoluteOrRelative)
  { 
    context.template path_elliptical_arc_to<PathPolicy>(rx, ry, x_axis_rotation, large_arc_flag, sweep_flag, x, y, absoluteOrRelative);
  }

  static void path_close_subpath(Adapter & context)
  { 
    context.template path_close_subpath<PathPolicy>(); 
  }

  static void path_exit(Adapter & context)
  { 
    context.template path_exit<PathPolicy>(); 
  }
};

template<class PathPolicy>
struct need_path_adapter: boost::mpl::bool_<
  PathPolicy::absolute_coordinates_only 
  || PathPolicy::no_ortho_line_to
  || PathPolicy::no_quadratic_bezier_shorthand 
  || PathPolicy::no_cubic_bezier_shorthand 
  || PathPolicy::quadratic_bezier_as_cubic>
{};

template<class OriginalContext, class Enabled = void>
struct path_adapter_if_needed
{
private:
  struct adapter_stub
  {
    template<class Context> adapter_stub(Context const &) {}
  };

public:
  typedef adapter_stub type;
  typedef OriginalContext adapted_context;
  typedef OriginalContext & adapted_context_holder;

  static OriginalContext & adapt_context(OriginalContext & context, adapter_stub &)
  {
    return context;
  }
};

template<class OriginalContext>
struct path_adapter_if_needed<OriginalContext, 
  typename boost::enable_if<need_path_adapter<typename detail::unwrap_context<OriginalContext, tag::path_policy>::policy> >::type>
{
private:
  typedef typename detail::unwrap_context<OriginalContext, tag::path_policy>::policy path_policy;
  typedef typename detail::unwrap_context<OriginalContext, tag::path_events_policy>::policy original_path_events_policy;
  typedef typename detail::unwrap_context<OriginalContext, tag::number_type>::policy number_type;

public:
  typedef path_adapter<
    typename original_path_events_policy::context_type, 
    path_policy, 
    number_type, 
    original_path_events_policy
  > type;
  typedef const adapted_context_wrapper<
    OriginalContext, 
    type, 
    tag::path_events_policy, 
    path_adapter_path_events_policy<type, path_policy, number_type>
  > adapted_context;
  typedef adapted_context adapted_context_holder;

  static adapted_context adapt_context(OriginalContext & context, type & adapter)
  {
    return adapted_context(context, adapter);
  }
};

}

}
