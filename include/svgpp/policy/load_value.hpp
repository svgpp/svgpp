#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/traits/inherited_property.hpp>

namespace svgpp { namespace policy { namespace load_value 
{

template<class Context>
struct forward_to_method
{
  template<class AttributeTag, class T1>
  static void set(Context & context, AttributeTag tag, T1 const & value)
  {
    context.set(tag, value);
  }

  template<class AttributeTag, class T1>
  static void set(Context & context, AttributeTag tag, T1 const & value, tag::source::attribute)
  {
    context.set(tag, value);
  }

  template<class AttributeTag, class T1>
  static void set(Context & context, AttributeTag tag, T1 const & value, tag::source::css)
  {
    context.set(tag, value);
  }

  template<class AttributeTag, class T1, class T2>
  static void set(Context & context, AttributeTag tag, T1 const & value1, T2 const & value2)
  {
    context.set(tag, value1, value2);
  }

  template<class AttributeTag, class T1, class T2, class T3>
  static void set(Context & context, AttributeTag tag, T1 const & value1, T2 const & value2, T3 const & value3)
  {
    context.set(tag, value1, value2, value3);
  }

  template<class AttributeTag, class T1, class T2, class T3, class T4>
  static void set(Context & context, AttributeTag tag, T1 const & value1, T2 const & value2, T3 const & value3, 
    T4 const & value4)
  {
    context.set(tag, value1, value2, value3, value4);
  }

  template<class Coordinate>
  static void set_viewport(Context & context, Coordinate translate_x, Coordinate translate_y, 
    Coordinate scale_x, Coordinate scale_y, bool defer)
  {
    context.set_viewport(translate_x, translate_y, scale_x, scale_y, defer);
  }

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