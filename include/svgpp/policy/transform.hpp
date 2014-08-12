// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/number_type.hpp>

namespace svgpp { namespace policy { namespace transform
{

struct raw
{
  static const bool join_transforms = false;
  static const bool no_rotate_about_point = false;
  static const bool no_shorthands = false; // Replace translate(tx) with translate(tx 0) and scale(scale) with scale(scale scale)
  static const bool only_matrix_transform = false;
};

struct minimal
{
  static const bool join_transforms = false;
  static const bool no_rotate_about_point = true;
  static const bool no_shorthands = true;
  static const bool only_matrix_transform = false;
};

struct matrix
{
  static const bool join_transforms = true;
  static const bool no_rotate_about_point = false;
  static const bool no_shorthands = false; 
  static const bool only_matrix_transform = false;
};

typedef matrix default_policy;

template<class Context>
struct by_context
{
  typedef default_policy type;
};

}}}