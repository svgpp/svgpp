// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>

namespace svgpp { namespace traits {

template<class AttributeTag> 
struct length_dimension_by_attribute{ typedef tag::length_dimension::not_width_nor_height type; };
template<> struct length_dimension_by_attribute<tag::attribute::x>            { typedef tag::length_dimension::width  type; };
template<> struct length_dimension_by_attribute<tag::attribute::y>            { typedef tag::length_dimension::height type; };
template<> struct length_dimension_by_attribute<tag::attribute::x1>           { typedef tag::length_dimension::width  type; };
template<> struct length_dimension_by_attribute<tag::attribute::y1>           { typedef tag::length_dimension::height type; };
template<> struct length_dimension_by_attribute<tag::attribute::x2>           { typedef tag::length_dimension::width  type; };
template<> struct length_dimension_by_attribute<tag::attribute::y2>           { typedef tag::length_dimension::height type; };
template<> struct length_dimension_by_attribute<tag::attribute::width>        { typedef tag::length_dimension::width  type; };    
template<> struct length_dimension_by_attribute<tag::attribute::height>       { typedef tag::length_dimension::height type; };
template<> struct length_dimension_by_attribute<tag::attribute::rx>           { typedef tag::length_dimension::width  type; };
template<> struct length_dimension_by_attribute<tag::attribute::ry>           { typedef tag::length_dimension::height type; };
template<> struct length_dimension_by_attribute<tag::attribute::cx>           { typedef tag::length_dimension::width  type; };
template<> struct length_dimension_by_attribute<tag::attribute::cy>           { typedef tag::length_dimension::height type; };
template<> struct length_dimension_by_attribute<tag::attribute::refX>         { typedef tag::length_dimension::width  type; };
template<> struct length_dimension_by_attribute<tag::attribute::refY>         { typedef tag::length_dimension::height type; };
template<> struct length_dimension_by_attribute<tag::attribute::markerWidth>  { typedef tag::length_dimension::width  type; };
template<> struct length_dimension_by_attribute<tag::attribute::markerHeight> { typedef tag::length_dimension::height type; };

}}