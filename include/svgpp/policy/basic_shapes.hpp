// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <boost/mpl/set.hpp>

namespace svgpp { namespace policy { namespace basic_shapes
{

struct raw
{
  typedef boost::mpl::set0<> convert_to_path;
  typedef boost::mpl::set0<> collect_attributes;

  static const bool convert_only_rounded_rect_to_path = false;
};

struct all_to_path
{
  typedef boost::mpl::set6<
    tag::element::rect,
    tag::element::line,
    tag::element::circle,
    tag::element::ellipse,
    tag::element::polyline,
    tag::element::polygon
  > convert_to_path;

  typedef boost::mpl::set0<> collect_attributes;

  static const bool convert_only_rounded_rect_to_path = false;
};

typedef all_to_path default_policy;

template<class Context>
struct by_context
{
  typedef default_policy type;
};

}}}