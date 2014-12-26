// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/traits/inherited_property.hpp>

namespace svgpp { namespace policy { namespace value_events 
{

template<class Context>
struct forward_to_method
{
#ifndef BOOST_NO_CXX11_VARIADIC_TEMPLATES
  template<class AttributeTag, class... Args>
  static void set(Context & context, AttributeTag tag, tag::source::any const &, const Args&... args)
  {
    context.set(tag, args...);
  }
#else
  template<class AttributeTag, class T1>
  static void set(Context & context, AttributeTag tag, tag::source::any const &, T1 const & value)
  {
    context.set(tag, value);
  }

  template<class AttributeTag, class T1, class T2>
  static void set(Context & context, AttributeTag tag, tag::source::any const &, T1 const & value1, T2 const & value2)
  {
    context.set(tag, value1, value2);
  }

  template<class AttributeTag, class T1, class T2, class T3>
  static void set(Context & context, AttributeTag tag, tag::source::any const &, T1 const & value1, T2 const & value2, T3 const & value3)
  {
    context.set(tag, value1, value2, value3);
  }

  template<class AttributeTag, class T1, class T2, class T3, class T4>
  static void set(Context & context, AttributeTag tag, tag::source::any const &, T1 const & value1, T2 const & value2, T3 const & value3, 
    T4 const & value4)
  {
    context.set(tag, value1, value2, value3, value4);
  }

  template<class AttributeTag, class T1, class T2, class T3, class T4, class T5>
  static void set(Context & context, AttributeTag tag, tag::source::any const &, T1 const & value1, T2 const & value2, T3 const & value3, 
    T4 const & value4, T5 const & value5)
  {
    context.set(tag, value1, value2, value3, value4);
  }

  template<class AttributeTag, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  static void set(Context & context, AttributeTag tag, tag::source::any const &, 
    T1 const & value1, T2 const & value2, T3 const & value3, 
    T4 const & value4, T5 const & value5, T6 const & value6, 
    T7 const & value7, T8 const & value8)
  {
    context.set(tag, value1, value2, value3, value4, value5, value6, value7, value8);
  }
#endif // BOOST_NO_CXX11_VARIADIC_TEMPLATES
};

template<class Context, class Base>
struct skip_inherit: Base
{
  using Base::set;

  template<class AttributeTag>
  static void set(Context &, AttributeTag, tag::source::any const &, tag::value::inherit,
    typename boost::enable_if<traits::inherited_property<AttributeTag> >::type * = NULL)
  {}

  template<class AttributeTag>
  static void set(Context &, AttributeTag, tag::source::any const &, tag::value::inherit, tag::source::attribute,
    typename boost::enable_if<traits::inherited_property<AttributeTag> >::type * = NULL)
  {}

  template<class AttributeTag>
  static void set(Context &, AttributeTag, tag::source::any const &, tag::value::inherit, tag::source::css,
    typename boost::enable_if<traits::inherited_property<AttributeTag> >::type * = NULL)
  {}
};

template<class Context>
struct default_policy: skip_inherit<Context, forward_to_method<Context> >
{};

}}}