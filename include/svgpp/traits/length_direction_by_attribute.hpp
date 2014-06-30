#pragma once

#include <svgpp/definitions.hpp>

namespace svgpp { namespace traits {

template<class AttributeTag> 
struct length_direction_by_attribute{ typedef tag::not_width_nor_height_length type; };
template<> struct length_direction_by_attribute<tag::attribute::x>            { typedef tag::width_length  type; };
template<> struct length_direction_by_attribute<tag::attribute::y>            { typedef tag::height_length type; };
template<> struct length_direction_by_attribute<tag::attribute::x1>           { typedef tag::width_length  type; };
template<> struct length_direction_by_attribute<tag::attribute::y1>           { typedef tag::height_length type; };
template<> struct length_direction_by_attribute<tag::attribute::x2>           { typedef tag::width_length  type; };
template<> struct length_direction_by_attribute<tag::attribute::y2>           { typedef tag::height_length type; };
template<> struct length_direction_by_attribute<tag::attribute::width>        { typedef tag::width_length  type; };    
template<> struct length_direction_by_attribute<tag::attribute::height>       { typedef tag::height_length type; };
template<> struct length_direction_by_attribute<tag::attribute::rx>           { typedef tag::width_length  type; };
template<> struct length_direction_by_attribute<tag::attribute::ry>           { typedef tag::height_length type; };
template<> struct length_direction_by_attribute<tag::attribute::cx>           { typedef tag::width_length  type; };
template<> struct length_direction_by_attribute<tag::attribute::cy>           { typedef tag::height_length type; };
template<> struct length_direction_by_attribute<tag::attribute::refX>         { typedef tag::width_length  type; };
template<> struct length_direction_by_attribute<tag::attribute::refY>         { typedef tag::height_length type; };
template<> struct length_direction_by_attribute<tag::attribute::markerWidth>  { typedef tag::width_length  type; };
template<> struct length_direction_by_attribute<tag::attribute::markerHeight> { typedef tag::height_length type; };

}}