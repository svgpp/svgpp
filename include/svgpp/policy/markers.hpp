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

namespace svgpp { 
  
// TODO: class enums for C++11
enum marker_vertex { marker_start = 1, marker_mid = 2, marker_end = 3 };
enum marker_config { marker_none = 0, marker_orient_ignore = 1, marker_orient_auto = 2 };
  
namespace policy 
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
        if (dir < 0)
          return dir + boost::math::constants::pi<directionality_type>();
        else
          return dir - boost::math::constants::pi<directionality_type>();
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

  struct default_policy
  {
    static const bool calculate_markers = false;
  };

  template<class Context>
  struct by_context
  {
    typedef default_policy type;
  };

} // namespace markers
  
namespace load_markers
{

  template<class Context>
  struct forward_to_method
  {
    typedef Context context_type;

    template<class Coordinate, class Directionality>
    static void marker(Context & context, marker_vertex v, 
      Coordinate x, Coordinate y, Directionality directionality, unsigned marker_index)
    {
      context.marker(v, x, y, directionality, marker_index);
    }

    static void marker_get_config(Context & context, marker_config & start, marker_config & mid, marker_config & end)
    {
      context.marker_get_config(start, mid, end);
    }
  };

  template<class Context>
  struct default_policy: forward_to_method<Context>
  {};

} // namespace load_markers

}}