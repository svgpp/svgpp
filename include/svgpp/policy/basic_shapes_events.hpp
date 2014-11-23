// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

namespace svgpp { namespace policy { namespace basic_shapes_events
{

template<class Context>
struct forward_to_method
{
  template<class Coordinate>
  static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height,
    Coordinate rx, Coordinate ry)
  {
    context.set_rect(x, y, width, height, rx, ry);
  }

  template<class Coordinate>
  static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height)
  {
    context.set_rect(x, y, width, height);
  }

  template<class Coordinate>
  static void set_line(Context & context, Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2)
  {
    context.set_line(x1, y1, x2, y2);
  }

  template<class Coordinate>
  static void set_circle(Context & context, Coordinate cx, Coordinate cy, Coordinate r)
  {
    context.set_circle(cx, cy, r);
  }

  template<class Coordinate>
  static void set_ellipse(Context & context, Coordinate cx, Coordinate cy, Coordinate rx, Coordinate ry)
  {
    context.set_ellipse(cx, cy, rx, ry);
  }
};

template<class Context>
struct default_policy: forward_to_method<Context>
{};

}}}