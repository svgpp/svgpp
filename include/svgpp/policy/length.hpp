// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/factory/unitless_length.hpp>

namespace svgpp { namespace policy { namespace length
{

template<class Context, class LengthFactory = const typename Context::length_factory_type>
struct forward_to_method
{
  typedef LengthFactory length_factory_type;

  static length_factory_type & length_factory(Context & context)
  {
    return context.length_factory();
  }
};

namespace
{
  factory::length::unitless<> const default_factory_instance;
}

template<class Context>
struct default_policy
{
  typedef factory::length::unitless<> const length_factory_type;

  static length_factory_type & length_factory(Context const &)
  {
    return default_factory_instance;
  }
};

}}}