// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <cmath>
#include <boost/config.hpp>
#include <boost/math/constants/constants.hpp>

namespace svgpp { namespace policy 
{ 
  
namespace marker_directionality
{

template<class Number = double>
struct radians
{
  typedef Number directionality_type;

  static BOOST_CONSTEXPR directionality_type undetermined_directionality() // "align with the positive x-axis in user space"
  {
    return 0;
  }

  template<class Coordinate>
  static directionality_type segment_directionality(Coordinate dx, Coordinate dy)
  {
    return std::atan2(dy, dx);
  }

  static directionality_type bisector_directionality(directionality_type in_segment, directionality_type out_segment)
  {
    directionality_type dir = (in_segment + out_segment) * 0.5;
    if (std::fabs(in_segment - out_segment) > boost::math::constants::pi<directionality_type>())
    {
      if (dir < 0)
        return dir + boost::math::constants::pi<directionality_type>();
      else
        return dir - boost::math::constants::pi<directionality_type>();
    }
    return dir;
  }
};

} // namespace marker_directionality

namespace markers
{

struct calculate
{
  typedef marker_directionality::radians<> directionality_policy;

  static const bool calculate_markers = true;
  static const bool always_calculate_auto_orient = false; // Doesn't call marker_get_config if true
};

struct calculate_always
{
  typedef marker_directionality::radians<> directionality_policy;

  static const bool calculate_markers = true;
  static const bool always_calculate_auto_orient = true; // Doesn't call marker_get_config if true
};

struct raw
{
  static const bool calculate_markers = false;
};

typedef raw default_policy;

template<class Context>
struct by_context
{
  typedef default_policy type;
};

}}}
