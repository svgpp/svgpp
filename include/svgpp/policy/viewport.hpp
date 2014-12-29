// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

namespace svgpp { namespace policy { namespace viewport
{

struct raw
{
  static const bool calculate_viewport = false;
  static const bool calculate_marker_viewport = false;
  static const bool calculate_pattern_viewport = false;
  static const bool viewport_as_transform = false;
};

struct as_transform
{
  static const bool calculate_viewport = true;
  static const bool calculate_marker_viewport = true;
  static const bool calculate_pattern_viewport = true;
  static const bool viewport_as_transform = true;
};

struct calculate
{
  static const bool calculate_viewport = true;
  static const bool calculate_marker_viewport = true;
  static const bool calculate_pattern_viewport = true;
  static const bool viewport_as_transform = false;
};

typedef raw default_policy;

template<class Context>
struct by_context
{
  typedef default_policy type;
};

}}}