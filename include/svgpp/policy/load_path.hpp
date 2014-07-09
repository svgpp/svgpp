#pragma once

#include <svgpp/policy/detail/same_object_func.hpp>

namespace svgpp { namespace policy { namespace load_path
{

template<class Context, class GetObject = detail::same_object_func<Context> >
struct forward_to_method
{
  template<class Coordinate, class AbsoluteOrRelative>
  static void path_move_to(Context & context, Coordinate x, Coordinate y, AbsoluteOrRelative absoluteOrRelative)
  { 
    GetObject::call(context).path_move_to(x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_line_to(Context & context, Coordinate x, Coordinate y, AbsoluteOrRelative absoluteOrRelative)
  { 
    GetObject::call(context).path_line_to(x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_line_to_ortho(Context & context, Coordinate coord, bool horizontal, AbsoluteOrRelative absoluteOrRelative)
  { 
    GetObject::call(context).path_line_to_ortho(coord, horizontal, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_cubic_bezier_to(Context & context, Coordinate x1, Coordinate y1, 
                                        Coordinate x2, Coordinate y2, 
                                        Coordinate x, Coordinate y, 
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    GetObject::call(context).path_cubic_bezier_to(x1, y1, x2, y2, x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_cubic_bezier_to(Context & context, 
                                        Coordinate x2, Coordinate y2, 
                                        Coordinate x, Coordinate y, 
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    GetObject::call(context).path_cubic_bezier_to(x2, y2, x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_quadratic_bezier_to(Context & context, 
                                        Coordinate x1, Coordinate y1, 
                                        Coordinate x, Coordinate y, 
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    GetObject::call(context).path_quadratic_bezier_to(x1, y1, x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_quadratic_bezier_to(Context & context, 
                                        Coordinate x, Coordinate y, 
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    GetObject::call(context).path_quadratic_bezier_to(x, y, absoluteOrRelative); 
  }

  template<class Coordinate, class AbsoluteOrRelative>
  static void path_elliptical_arc_to(Context & context, 
                                        Coordinate rx, Coordinate ry, Coordinate x_axis_rotation,
                                        bool large_arc_flag, bool sweep_flag, 
                                        Coordinate x, Coordinate y,
                                        AbsoluteOrRelative absoluteOrRelative)
  { 
    GetObject::call(context).path_elliptical_arc_to(rx, ry, x_axis_rotation, large_arc_flag, sweep_flag, x, y, absoluteOrRelative); 
  }

  static void path_close_subpath(Context & context)
  { 
    GetObject::call(context).path_close_subpath(); 
  }

  static void path_exit(Context & context)
  { 
    GetObject::call(context).path_exit(); 
  }
};

}}}