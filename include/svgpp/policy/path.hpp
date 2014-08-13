// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

namespace svgpp { namespace policy { namespace path
{

struct raw
{
  static const bool absolute_coordinates_only = false;
  static const bool no_ortho_line_to = false;
  static const bool no_quadratic_bezier_shorthand = false;
  static const bool no_cubic_bezier_shorthand = false;
  static const bool quadratic_bezier_as_cubic = false;
  static const bool arc_as_cubic_bezier = false; 
  // TODO: only not axis aligned ellipse arc as bezier
  // TODO: arc as center and radius
  // TODO: pass explicit move_to on close path?
  // TODO: fix arc errors
};

struct no_shorthands
{
  static const bool absolute_coordinates_only = true;
  static const bool no_ortho_line_to = true;
  static const bool no_quadratic_bezier_shorthand = true;
  static const bool no_cubic_bezier_shorthand = true;
  static const bool quadratic_bezier_as_cubic = false;
  static const bool arc_as_cubic_bezier = false; 
};

struct minimal
{
  static const bool absolute_coordinates_only = true;
  static const bool no_ortho_line_to = true;
  static const bool no_quadratic_bezier_shorthand = true;
  static const bool no_cubic_bezier_shorthand = true;
  static const bool quadratic_bezier_as_cubic = true;
  static const bool arc_as_cubic_bezier = true; 
};

typedef no_shorthands default_policy;

template<class Context>
struct by_context
{
  typedef default_policy type;
};

}}}