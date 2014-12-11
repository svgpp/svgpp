// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <cmath>

namespace svgpp 
{ 
  
enum marker_vertex { marker_start = 1, marker_mid = 2, marker_end = 3 };
enum marker_config { marker_none = 0, marker_orient_fixed = 1, marker_orient_auto = 2 };
 
namespace policy { namespace marker_events
{

template<class Context>
struct forward_to_method
{
  typedef Context context_type;

  template<class Coordinate, class Directionality>
  static void marker(Context & context, marker_vertex v, 
    Coordinate x, Coordinate y, Directionality directionality, unsigned marker_index)
  {
    context.marker(v, x, y, directionality, marker_index);
  }

  static void marker_get_config(Context & context, marker_config & start, marker_config & mid, marker_config & end)
  {
    context.marker_get_config(start, mid, end);
  }
};

template<class Context>
struct default_policy: forward_to_method<Context>
{};

}}}
