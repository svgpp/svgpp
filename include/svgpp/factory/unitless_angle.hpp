// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/traits/angle_units.hpp>

namespace svgpp { namespace factory { namespace angle 
{

template<class Angle = double, class ReferenceAngleUnits = tag::angle_units::deg>
struct unitless
{
  typedef Angle angle_type;

  template<class Number>
  static Angle create(Number value, ReferenceAngleUnits)
  {
    return static_cast<Angle>(value);
  }

  template<class Number, class Units>
  static Angle create(Number value, Units)
  {
    return static_cast<Angle>(value) * traits::angle_conversion_coefficient<Units, ReferenceAngleUnits, Angle>::value();
  }
};

}}}