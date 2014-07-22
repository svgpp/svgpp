// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/array.hpp>

namespace svgpp { namespace policy { namespace load_transform 
{

template<class Context>
struct forward_to_method
{
  typedef Context context_type; // May be used to reduce number of instantiations

  template<class Number>
  static void set_transform_matrix(Context & context, const boost::array<Number, 6> & matrix)
  {
    context.set_transform_matrix(matrix);
  }

  template<class Number>
  static void append_transform_matrix(Context & context, const boost::array<Number, 6> & matrix)
  {
    context.append_transform_matrix(matrix);
  }

  template<class Number>
  static void append_transform_translate(Context & context, Number tx, Number ty)
  {
    context.append_transform_translate(tx, ty);
  }

  template<class Number>
  static void append_transform_translate(Context & context, Number tx)
  {
    context.append_transform_translate(tx);
  }

  template<class Number>
  static void append_transform_scale(Context & context, Number sx, Number sy)
  {
    context.append_transform_scale(sx, sy);
  }

  template<class Number>
  static void append_transform_scale(Context & context, Number scale)
  {
    context.append_transform_scale(scale);
  }

  template<class Number>
  static void append_transform_rotate(Context & context, Number angle)
  {
    context.append_transform_rotate(angle);
  }

  template<class Number>
  static void append_transform_rotate(Context & context, Number angle, Number cx, Number cy)
  {
    context.append_transform_rotate(angle, cx, cy);
  }

  template<class Number>
  static void append_transform_skew_x(Context & context, Number angle)
  {
    context.append_transform_skew_x(angle);
  }

  template<class Number>
  static void append_transform_skew_y(Context & context, Number angle)
  {
    context.append_transform_skew_y(angle);
  }
};

template<class Context>
struct default_policy: forward_to_method<Context>
{};

}}}