// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

namespace svgpp { namespace detail
{

template<class Context, class PathEventsPolicy1, class PathEventsPolicy2>
struct path_events_splitter
{
  typedef Context context_type;

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_move_to(Context & context, Coordinate x, Coordinate y, AbsoluteOrRelative absoluteOrRelative)
  { 
    PathEventsPolicy1::path_move_to(context.first , x, y, absoluteOrRelative); 
    PathEventsPolicy2::path_move_to(context.second, x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_line_to(Context & context, Coordinate x, Coordinate y, AbsoluteOrRelative absoluteOrRelative)
  { 
    PathEventsPolicy1::path_line_to(context.first , x, y, absoluteOrRelative); 
    PathEventsPolicy2::path_line_to(context.second, x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_line_to_ortho(Context & context, Coordinate coord, bool horizontal, AbsoluteOrRelative absoluteOrRelative)
  { 
    PathEventsPolicy1::path_line_to_ortho(context.first , coord, horizontal, absoluteOrRelative); 
    PathEventsPolicy2::path_line_to_ortho(context.second, coord, horizontal, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_cubic_bezier_to(Context & context, Coordinate x1, Coordinate y1, 
                                        Coordinate x2, Coordinate y2, 
                                        Coordinate x, Coordinate y, 
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    PathEventsPolicy1::path_cubic_bezier_to(context.first , x1, y1, x2, y2, x, y, absoluteOrRelative); 
    PathEventsPolicy2::path_cubic_bezier_to(context.second, x1, y1, x2, y2, x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_cubic_bezier_to(Context & context, 
                                        Coordinate x2, Coordinate y2, 
                                        Coordinate x, Coordinate y, 
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    PathEventsPolicy1::path_cubic_bezier_to(context.first , x2, y2, x, y, absoluteOrRelative); 
    PathEventsPolicy2::path_cubic_bezier_to(context.second, x2, y2, x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_quadratic_bezier_to(Context & context, 
                                        Coordinate x1, Coordinate y1, 
                                        Coordinate x, Coordinate y, 
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    PathEventsPolicy1::path_quadratic_bezier_to(context.first , x1, y1, x, y, absoluteOrRelative); 
    PathEventsPolicy2::path_quadratic_bezier_to(context.second, x1, y1, x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_quadratic_bezier_to(Context & context, 
                                        Coordinate x, Coordinate y, 
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    PathEventsPolicy1::path_quadratic_bezier_to(context.first , x, y, absoluteOrRelative); 
    PathEventsPolicy2::path_quadratic_bezier_to(context.second, x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_elliptical_arc_to(Context & context, 
                                        Coordinate rx, Coordinate ry, Coordinate x_axis_rotation,
                                        bool large_arc_flag, bool sweep_flag, 
                                        Coordinate x, Coordinate y,
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    PathEventsPolicy1::path_elliptical_arc_to(context.first , rx, ry, x_axis_rotation, large_arc_flag, sweep_flag, x, y, absoluteOrRelative); 
    PathEventsPolicy2::path_elliptical_arc_to(context.second, rx, ry, x_axis_rotation, large_arc_flag, sweep_flag, x, y, absoluteOrRelative); 
  }

  static void path_close_subpath(Context & context)
  { 
    PathEventsPolicy1::path_close_subpath(context.first); 
    PathEventsPolicy2::path_close_subpath(context.second); 
  }

  static void path_exit(Context & context)
  { 
    PathEventsPolicy1::path_exit(context.first); 
    PathEventsPolicy2::path_exit(context.second); 
  }
};

}}