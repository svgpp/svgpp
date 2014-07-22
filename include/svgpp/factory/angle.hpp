// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/factory/unitless_angle.hpp>

namespace svgpp { namespace factory { namespace angle 
{

  typedef unitless<> default_factory;

  template<class Context>
  struct by_context
  {
    typedef default_factory type;
  };

}}}