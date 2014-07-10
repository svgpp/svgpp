#pragma once

#include <svgpp/number_type.hpp>

namespace svgpp { namespace policy { namespace transform
{

template<class Number>
struct raw
{
  typedef Number number_type;

  static const bool join_transforms = false;
  static const bool no_rotate_about_point = false;
  static const bool no_shorthands = false; // Replace translate(tx) with translate(tx 0) and scale(scale) with scale(scale scale)
  static const bool only_matrix_transform = false;
};

template<class Number>
struct matrix
{
  typedef Number number_type;

  static const bool join_transforms = true;
  static const bool no_rotate_about_point = false;
  static const bool no_shorthands = false; 
  static const bool only_matrix_transform = false;
};

template<class Number>
struct default_policy: matrix<Number>
{};

template<class Context>
struct by_context
{
  typedef default_policy<typename number_type_by_context<Context>::type> type;
};

}}}