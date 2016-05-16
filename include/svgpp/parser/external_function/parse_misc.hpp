// Copyright Oleg Maximenko 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>

namespace svgpp { namespace detail 
{

template<class Iterator, class Coordinate>
bool parse_viewBox(Iterator & it, Iterator end, Coordinate & x, Coordinate & y, Coordinate & w, Coordinate & h);

template<class Iterator, class Coordinate>
bool parse_bbox(Iterator & it, Iterator end, Coordinate & lo_x, Coordinate & lo_y, Coordinate & hi_x, Coordinate & hi_y);

template<class Iterator, class PropertySource, class Coordinate>
bool parse_enable_background(Iterator & it, Iterator end, 
  PropertySource,
  Coordinate & x, Coordinate & y, Coordinate & w, Coordinate & h);

template<class LengthFactory, class Iterator, class PropertySource>
bool parse_clip(
  LengthFactory const & length_factory,
  Iterator & it, Iterator end,
  PropertySource property_source,
  typename LengthFactory::length_type * out_rect);

}}