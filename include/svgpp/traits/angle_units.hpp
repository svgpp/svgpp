#pragma once

#include <svgpp/definitions.hpp>
#include <boost/math/constants/constants.hpp>

namespace svgpp { namespace traits
{

template<class Src, class Dst, class Number = double>
struct angle_conversion_coefficient
{
  static BOOST_CONSTEXPR Number value() 
  { 
    return angle_conversion_coefficient<Src, tag::angle_units::deg>::value() 
      / angle_conversion_coefficient<Dst, tag::angle_units::deg>::value();
  }
};

template<class Src, class Number>
struct angle_conversion_coefficient<Src, Src, Number>
{ static BOOST_CONSTEXPR Number value() { return static_cast<Number>(1); } };

template<class Number>
struct angle_conversion_coefficient<tag::angle_units::grad, tag::angle_units::deg, Number>
{ static BOOST_CONSTEXPR Number value() { return static_cast<Number>(0.9); } };

template<class Number>
struct angle_conversion_coefficient<tag::angle_units::rad, tag::angle_units::deg, Number>
{ static BOOST_CONSTEXPR Number value() { return boost::math::constants::radian<Number>(); } };

}}