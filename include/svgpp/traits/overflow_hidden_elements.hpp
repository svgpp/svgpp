// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <boost/mpl/set.hpp>

namespace svgpp { namespace traits
{

/*
6.16 User agent style sheet

The user agent shall maintain a user agent style sheet ([CSS2], section 6.4) for elements in the SVG namespace 
for visual media ([CSS2], section 7.3.1). The user agent style sheet below is expressed using CSS syntax; however, 
user agents are required to support the behavior that corresponds to this default style sheet even if CSS style 
sheets are not supported in the user agent:
svg, symbol, image, marker, pattern, foreignObject { overflow: hidden }
...

The first line of the above user agent style sheet will cause the initial clipping path to be established at the 
bounds of the initial viewport. Furthermore, it will cause new clipping paths to be established at the bounds of 
the listed elements, all of which are elements that establish a new viewport. (Refer to the description of SVG's 
use of the 'overflow' property for more information.)
*/

typedef boost::mpl::set6<
  tag::element::svg, 
  tag::element::symbol, 
  tag::element::image, 
  tag::element::marker, 
  tag::element::pattern, 
  tag::element::foreignObject
> default_overflow_hidden_elements;

}}