#pragma once

#include <boost/array.hpp>
#include <svgpp/policy/detail/same_object_func.hpp>

namespace svgpp { namespace policy { namespace load_transform 
{

template<class Context, class GetObject = detail::same_object_func<Context> >
struct forward_to_method
{
  typedef Context context_type; // May be used to reduce number of instantiations

  template<class Number>
  static void set_transform_matrix(Context & context, const boost::array<Number, 6> & matrix)
  {
    GetObject::call(context).set_transform_matrix(matrix);
  }

  template<class Number>
  static void append_transform_matrix(Context & context, const boost::array<Number, 6> & matrix)
  {
    GetObject::call(context).append_transform_matrix(matrix);
  }

  template<class Number>
  static void append_transform_translate(Context & context, Number tx, Number ty)
  {
    GetObject::call(context).append_transform_translate(tx, ty);
  }

  template<class Number>
  static void append_transform_translate(Context & context, Number tx)
  {
    GetObject::call(context).append_transform_translate(tx);
  }

  template<class Number>
  static void append_transform_scale(Context & context, Number sx, Number sy)
  {
    GetObject::call(context).append_transform_scale(sx, sy);
  }

  template<class Number>
  static void append_transform_scale(Context & context, Number scale)
  {
    GetObject::call(context).append_transform_scale(scale);
  }

  template<class Number>
  static void append_transform_rotate(Context & context, Number angle)
  {
    GetObject::call(context).append_transform_rotate(angle);
  }

  template<class Number>
  static void append_transform_rotate(Context & context, Number angle, Number cx, Number cy)
  {
    GetObject::call(context).append_transform_rotate(angle, cx, cy);
  }

  template<class Number>
  static void append_transform_skew_x(Context & context, Number angle)
  {
    GetObject::call(context).append_transform_skew_x(angle);
  }

  template<class Number>
  static void append_transform_skew_y(Context & context, Number angle)
  {
    GetObject::call(context).append_transform_skew_y(angle);
  }
};

template<class Context>
struct default_policy: forward_to_method<Context>
{};

}}}