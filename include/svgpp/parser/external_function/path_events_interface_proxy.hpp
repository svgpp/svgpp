// Copyright Oleg Maximenko 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/parser/external_function/parse_path_data.hpp>

namespace svgpp { namespace detail 
{

template<
  class Context,
  class Coordinate,
  class EventsPolicy 
>
class path_events_interface_proxy: public path_events_interface<Coordinate>
{
public:
  path_events_interface_proxy(Context & context)
    : m_context(context)
  {}

  virtual void path_move_to(Coordinate x, Coordinate y, tag::coordinate::absolute absoluteOrRelative)
  {
    EventsPolicy::path_move_to(m_context, x, y, absoluteOrRelative);
  }

  virtual void path_move_to(Coordinate x, Coordinate y, tag::coordinate::relative absoluteOrRelative)
  {
    EventsPolicy::path_move_to(m_context, x, y, absoluteOrRelative);
  }

  virtual void path_line_to(Coordinate x, Coordinate y, tag::coordinate::absolute absoluteOrRelative)
  {
    EventsPolicy::path_line_to(m_context, x, y, absoluteOrRelative);
  }

  virtual void path_line_to(Coordinate x, Coordinate y, tag::coordinate::relative absoluteOrRelative)
  {
    EventsPolicy::path_line_to(m_context, x, y, absoluteOrRelative);
  }

  virtual void path_line_to_ortho(Coordinate coord, bool horizontal, tag::coordinate::absolute absoluteOrRelative)
  {
    EventsPolicy::path_line_to_ortho(m_context, coord, horizontal, absoluteOrRelative);
  }

  virtual void path_line_to_ortho(Coordinate coord, bool horizontal, tag::coordinate::relative absoluteOrRelative)
  {
    EventsPolicy::path_line_to_ortho(m_context, coord, horizontal, absoluteOrRelative);
  }

  virtual void path_cubic_bezier_to(Coordinate x1, Coordinate y1, 
    Coordinate x2, Coordinate y2, 
    Coordinate x, Coordinate y, 
    tag::coordinate::absolute absoluteOrRelative)
  {
    EventsPolicy::path_cubic_bezier_to(m_context, x1, y1, x2, y2, x, y, absoluteOrRelative);
  }

  virtual void path_cubic_bezier_to(Coordinate x1, Coordinate y1, 
    Coordinate x2, Coordinate y2, 
    Coordinate x, Coordinate y, 
    tag::coordinate::relative absoluteOrRelative)
  {
    EventsPolicy::path_cubic_bezier_to(m_context, x1, y1, x2, y2, x, y, absoluteOrRelative);
  }

  virtual void path_cubic_bezier_to(
    Coordinate x2, Coordinate y2, 
    Coordinate x, Coordinate y, 
    tag::coordinate::absolute absoluteOrRelative)
  {
    EventsPolicy::path_cubic_bezier_to(m_context, x2, y2, x, y, absoluteOrRelative);
  }

  virtual void path_cubic_bezier_to(
    Coordinate x2, Coordinate y2, 
    Coordinate x, Coordinate y, 
    tag::coordinate::relative absoluteOrRelative)
  {
    EventsPolicy::path_cubic_bezier_to(m_context, x2, y2, x, y, absoluteOrRelative);
  }

  virtual void path_quadratic_bezier_to(
    Coordinate x1, Coordinate y1, 
    Coordinate x, Coordinate y, 
    tag::coordinate::absolute absoluteOrRelative)
  {
    EventsPolicy::path_quadratic_bezier_to(m_context, x1, y1, x, y, absoluteOrRelative);
  }

  virtual void path_quadratic_bezier_to(
    Coordinate x1, Coordinate y1, 
    Coordinate x, Coordinate y, 
    tag::coordinate::relative absoluteOrRelative)
  {
    EventsPolicy::path_quadratic_bezier_to(m_context, x1, y1, x, y, absoluteOrRelative);
  }

  virtual void path_quadratic_bezier_to(
    Coordinate x, Coordinate y, 
    tag::coordinate::absolute absoluteOrRelative)
  {
    EventsPolicy::path_quadratic_bezier_to(m_context, x, y, absoluteOrRelative);
  }

  virtual void path_quadratic_bezier_to(
    Coordinate x, Coordinate y, 
    tag::coordinate::relative absoluteOrRelative)
  {
    EventsPolicy::path_quadratic_bezier_to(m_context, x, y, absoluteOrRelative);
  }

  virtual void path_elliptical_arc_to(
    Coordinate rx, Coordinate ry, Coordinate x_axis_rotation,
    bool large_arc_flag, bool sweep_flag, 
    Coordinate x, Coordinate y,
    tag::coordinate::absolute absoluteOrRelative)
  {
    EventsPolicy::path_elliptical_arc_to(m_context, rx, ry, 
      x_axis_rotation, large_arc_flag, sweep_flag, x, y, 
      absoluteOrRelative);
  }

  virtual void path_elliptical_arc_to(
    Coordinate rx, Coordinate ry, Coordinate x_axis_rotation,
    bool large_arc_flag, bool sweep_flag, 
    Coordinate x, Coordinate y,
    tag::coordinate::relative absoluteOrRelative)
  {
    EventsPolicy::path_elliptical_arc_to(m_context, rx, ry, 
      x_axis_rotation, large_arc_flag, sweep_flag, x, y, 
      absoluteOrRelative);
  }

  virtual void path_close_subpath()
  {
    EventsPolicy::path_close_subpath(m_context);
  }

  virtual void path_exit()
  {
    EventsPolicy::path_exit(m_context);
  }

private:
  Context & m_context;
};

}}