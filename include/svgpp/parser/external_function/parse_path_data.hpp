// Copyright Oleg Maximenko 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>

namespace svgpp { namespace detail 
{

template<class Coordinate>
struct path_events_interface
{
  virtual void path_move_to(Coordinate x, Coordinate y, tag::coordinate::absolute) = 0;
  virtual void path_move_to(Coordinate x, Coordinate y, tag::coordinate::relative) = 0;

  virtual void path_line_to(Coordinate x, Coordinate y, tag::coordinate::absolute) = 0;
  virtual void path_line_to(Coordinate x, Coordinate y, tag::coordinate::relative) = 0;

  virtual void path_line_to_ortho(Coordinate coord, bool horizontal, tag::coordinate::absolute) = 0;
  virtual void path_line_to_ortho(Coordinate coord, bool horizontal, tag::coordinate::relative) = 0;

  virtual void path_cubic_bezier_to(Coordinate x1, Coordinate y1, 
    Coordinate x2, Coordinate y2, 
    Coordinate x, Coordinate y, 
    tag::coordinate::absolute) = 0;
  virtual void path_cubic_bezier_to(Coordinate x1, Coordinate y1, 
    Coordinate x2, Coordinate y2, 
    Coordinate x, Coordinate y, 
    tag::coordinate::relative) = 0;

  virtual void path_cubic_bezier_to(
    Coordinate x2, Coordinate y2, 
    Coordinate x, Coordinate y, 
    tag::coordinate::absolute) = 0;
  virtual void path_cubic_bezier_to(
    Coordinate x2, Coordinate y2, 
    Coordinate x, Coordinate y, 
    tag::coordinate::relative) = 0;

  virtual void path_quadratic_bezier_to(
    Coordinate x1, Coordinate y1, 
    Coordinate x, Coordinate y, 
    tag::coordinate::absolute) = 0;
  virtual void path_quadratic_bezier_to(
    Coordinate x1, Coordinate y1, 
    Coordinate x, Coordinate y, 
    tag::coordinate::relative) = 0;

  virtual void path_quadratic_bezier_to(
    Coordinate x, Coordinate y, 
    tag::coordinate::absolute) = 0;
  virtual void path_quadratic_bezier_to(
    Coordinate x, Coordinate y, 
    tag::coordinate::relative) = 0;

  virtual void path_elliptical_arc_to(
    Coordinate rx, Coordinate ry, Coordinate x_axis_rotation,
    bool large_arc_flag, bool sweep_flag, 
    Coordinate x, Coordinate y,
    tag::coordinate::absolute) = 0;
  virtual void path_elliptical_arc_to(
    Coordinate rx, Coordinate ry, Coordinate x_axis_rotation,
    bool large_arc_flag, bool sweep_flag, 
    Coordinate x, Coordinate y,
    tag::coordinate::relative) = 0;

  virtual void path_close_subpath() = 0;
  virtual void path_exit() = 0;
};

template<class Iterator, class Coordinate>
bool parse_path_data(Iterator & it, Iterator end, path_events_interface<Coordinate> & context);

}}