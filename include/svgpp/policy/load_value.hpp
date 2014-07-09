#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/policy/detail/same_object_func.hpp>
#include <svgpp/traits/inherited_property.hpp>

namespace svgpp { namespace policy { namespace load_value 
{

template<class Context, class GetObject = detail::same_object_func<Context> >
struct forward_to_method
{
  template<class AttributeTag, class T1>
  static void set(Context & context, AttributeTag tag, T1 const & value)
  {
    GetObject::call(context).set(tag, value);
  }

  template<class AttributeTag, class T1>
  static void set(Context & context, AttributeTag tag, T1 const & value, tag::source::attribute)
  {
    GetObject::call(context).set(tag, value);
  }

  template<class AttributeTag, class T1>
  static void set(Context & context, AttributeTag tag, T1 const & value, tag::source::css)
  {
    GetObject::call(context).set(tag, value);
  }

  template<class AttributeTag, class T1, class T2>
  static void set(Context & context, AttributeTag tag, T1 const & value1, T2 const & value2)
  {
    GetObject::call(context).set(tag, value1, value2);
  }

  template<class AttributeTag, class T1, class T2, class T3>
  static void set(Context & context, AttributeTag tag, T1 const & value1, T2 const & value2, T3 const & value3)
  {
    GetObject::call(context).set(tag, value1, value2, value3);
  }

  template<class AttributeTag, class T1, class T2, class T3, class T4>
  static void set(Context & context, AttributeTag tag, T1 const & value1, T2 const & value2, T3 const & value3, 
    T4 const & value4)
  {
    GetObject::call(context).set(tag, value1, value2, value3, value4);
  }

  template<class Coordinate>
  static void set_viewport(Context & context, Coordinate viewport_x, Coordinate viewport_y, 
    Coordinate viewport_width, Coordinate viewport_height)
  {
    GetObject::call(context).set_viewport(viewport_x, viewport_y, viewport_width, viewport_height);
  }

  template<class Coordinate>
  static void set_viewport_transform(Context & context, Coordinate translate_x, Coordinate translate_y, 
    Coordinate scale_x, Coordinate scale_y, bool defer)
  {
    GetObject::call(context).set_viewport_transform(translate_x, translate_y, scale_x, scale_y, defer);
  }

  template<class Coordinate>
  static void set_viewport_transform(Context & context, Coordinate translate_x, Coordinate translate_y)
  {
    GetObject::call(context).set_viewport_transform(translate_x, translate_y);
  }

  template<class Coordinate>
  static void get_reference_viewport_size(Context & context, Coordinate & viewport_width, Coordinate & viewport_height)
  {
    GetObject::call(context).get_reference_viewport_size(viewport_width, viewport_height);
  }

  template<class Coordinate>
  static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height,
    Coordinate rx, Coordinate ry)
  {
    GetObject::call(context).set_rect(x, y, width, height, rx, ry);
  }

  template<class Coordinate>
  static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height)
  {
    GetObject::call(context).set_rect(x, y, width, height);
  }

  template<class Coordinate>
  static void set_line(Context & context, Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2)
  {
    GetObject::call(context).set_line(x1, y1, x2, y2);
  }

  template<class Coordinate>
  static void set_circle(Context & context, Coordinate cx, Coordinate cy, Coordinate r)
  {
    GetObject::call(context).set_circle(cx, cy, r);
  }
};

template<class Context, class Base>
struct skip_inherit: Base
{
  using Base::set;

  template<class AttributeTag>
  static void set(Context &, AttributeTag, tag::value::inherit,
    typename boost::enable_if<traits::inherited_property<AttributeTag> >::type * = NULL)
  {}

  template<class AttributeTag>
  static void set(Context &, AttributeTag, tag::value::inherit, tag::source::attribute,
    typename boost::enable_if<traits::inherited_property<AttributeTag> >::type * = NULL)
  {}

  template<class AttributeTag>
  static void set(Context &, AttributeTag, tag::value::inherit, tag::source::css,
    typename boost::enable_if<traits::inherited_property<AttributeTag> >::type * = NULL)
  {}
};

template<class Context>
struct default_policy: skip_inherit<Context, forward_to_method<Context> >
{};

}}}