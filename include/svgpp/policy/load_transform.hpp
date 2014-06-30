#pragma once

#include <boost/array.hpp>

namespace svgpp { namespace policy { namespace load_transform 
{

template<class Context>
struct forward_to_method
{
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

}}}