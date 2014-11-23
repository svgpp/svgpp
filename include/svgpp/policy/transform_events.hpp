// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/array.hpp>

namespace svgpp { namespace policy { namespace transform_events 
{

template<class Context>
struct forward_to_method
{
  typedef Context context_type; // May be used to reduce number of instantiations

  template<class Number>
  static void transform_matrix(Context & context, const boost::array<Number, 6> & matrix)
  {
    context.transform_matrix(matrix);
  }

  template<class Number>
  static void transform_translate(Context & context, Number tx, Number ty)
  {
    context.transform_translate(tx, ty);
  }

  template<class Number>
  static void transform_translate(Context & context, Number tx)
  {
    context.transform_translate(tx);
  }

  template<class Number>
  static void transform_scale(Context & context, Number sx, Number sy)
  {
    context.transform_scale(sx, sy);
  }

  template<class Number>
  static void transform_scale(Context & context, Number scale)
  {
    context.transform_scale(scale);
  }

  template<class Number>
  static void transform_rotate(Context & context, Number angle)
  {
    context.transform_rotate(angle);
  }

  template<class Number>
  static void transform_rotate(Context & context, Number angle, Number cx, Number cy)
  {
    context.transform_rotate(angle, cx, cy);
  }

  template<class Number>
  static void transform_skew_x(Context & context, Number angle)
  {
    context.transform_skew_x(angle);
  }

  template<class Number>
  static void transform_skew_y(Context & context, Number angle)
  {
    context.transform_skew_y(angle);
  }
};

template<class Context>
struct default_policy: forward_to_method<Context>
{};

}}}