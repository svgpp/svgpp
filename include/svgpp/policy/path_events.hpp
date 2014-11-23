// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

namespace svgpp { namespace policy { namespace path_events
{

template<class Context>
struct forward_to_method
{
  typedef Context context_type;

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_move_to(Context & context, Coordinate x, Coordinate y, AbsoluteOrRelative absoluteOrRelative)
  { 
    context.path_move_to(x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_line_to(Context & context, Coordinate x, Coordinate y, AbsoluteOrRelative absoluteOrRelative)
  { 
    context.path_line_to(x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_line_to_ortho(Context & context, Coordinate coord, bool horizontal, AbsoluteOrRelative absoluteOrRelative)
  { 
    context.path_line_to_ortho(coord, horizontal, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_cubic_bezier_to(Context & context, Coordinate x1, Coordinate y1, 
                                        Coordinate x2, Coordinate y2, 
                                        Coordinate x, Coordinate y, 
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    context.path_cubic_bezier_to(x1, y1, x2, y2, x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_cubic_bezier_to(Context & context, 
                                        Coordinate x2, Coordinate y2, 
                                        Coordinate x, Coordinate y, 
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    context.path_cubic_bezier_to(x2, y2, x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_quadratic_bezier_to(Context & context, 
                                        Coordinate x1, Coordinate y1, 
                                        Coordinate x, Coordinate y, 
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    context.path_quadratic_bezier_to(x1, y1, x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_quadratic_bezier_to(Context & context, 
                                        Coordinate x, Coordinate y, 
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    context.path_quadratic_bezier_to(x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_elliptical_arc_to(Context & context, 
                                        Coordinate rx, Coordinate ry, Coordinate x_axis_rotation,
                                        bool large_arc_flag, bool sweep_flag, 
                                        Coordinate x, Coordinate y,
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    context.path_elliptical_arc_to(rx, ry, x_axis_rotation, large_arc_flag, sweep_flag, x, y, absoluteOrRelative); 
  }

  static void path_close_subpath(Context & context)
  { 
    context.path_close_subpath(); 
  }

  static void path_exit(Context & context)
  { 
    context.path_exit(); 
  }
};

template<class Context>
struct default_policy: forward_to_method<Context>
{};

}}}