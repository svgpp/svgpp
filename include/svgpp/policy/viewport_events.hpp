// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

namespace svgpp { namespace policy { namespace viewport_events
{

template<class Context>
struct forward_to_method
{
  template<class Coordinate>
  static void set_viewport(Context & context, Coordinate viewport_x, Coordinate viewport_y, 
    Coordinate viewport_width, Coordinate viewport_height)
  {
    context.set_viewport(viewport_x, viewport_y, viewport_width, viewport_height);
  }

  template<class Coordinate>
  static void set_viewbox_transform(Context & context, Coordinate translate_x, Coordinate translate_y, 
    Coordinate scale_x, Coordinate scale_y, bool defer)
  {
    context.set_viewbox_transform(translate_x, translate_y, scale_x, scale_y, defer);
  }

  template<class Coordinate>
  static void set_viewbox_size(Context & context, Coordinate viewbox_width, Coordinate viewbox_height)
  {
    context.set_viewbox_size(viewbox_width, viewbox_height);
  }

  template<class Coordinate>
  static void get_reference_viewport_size(Context & context, Coordinate & viewport_width, Coordinate & viewport_height)
  {
    context.get_reference_viewport_size(viewport_width, viewport_height);
  }

  static void disable_rendering(Context & context)
  {
    context.disable_rendering();
  }
};

template<class Context>
struct default_policy: forward_to_method<Context>
{};

}}}