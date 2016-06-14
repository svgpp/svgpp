// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/detail/attribute_id.hpp>
#include <svgpp/detail/element_type_id.hpp>
#include <boost/parameter.hpp>
#include <boost/mpl/vector.hpp>

namespace svgpp { namespace tag 
{ 
  
namespace element
{
  struct any { protected: any() {} };

#define SVGPP_ON(element_name, str) struct element_name: any { \
  static const detail::element_type_id element_id = detail::element_type_id_## element_name; };
#include <svgpp/detail/dict/enumerate_all_elements.inc>
#undef SVGPP_ON
} // namespace element

namespace attribute
{
#define SVGPP_ON(attribute_name, attribute_string) \
  struct attribute_name { \
    static const detail::attribute_id attribute_id = detail::attribute_id_## attribute_name; };
#define SVGPP_ON_NS(ns, attribute_name, attribute_string) \
  namespace ns { struct attribute_name { \
    static const detail::attribute_id attribute_id = detail::attribute_id_ ## ns ## _ ## attribute_name; }; }
#define SVGPP_ON_STYLE(attribute_name, attribute_string) SVGPP_ON(attribute_name, attribute_string)
#include <svgpp/detail/dict/enumerate_all_attributes.inc>
#undef SVGPP_ON
#undef SVGPP_ON_NS
#undef SVGPP_ON_STYLE
} // namespace attribute

namespace length_units
{
  struct em {};
  struct ex {};
  struct px {};
  struct in {};
  struct cm {};
  struct mm {};
  struct pt {};
  struct pc {};
  struct percent {};
  struct none {};
}

namespace angle_units
{
  struct deg {};
  struct grad {};
  struct rad {};
}

namespace source
{
  struct any { protected: any() {} };
  struct attribute: any {};
  struct css: any {};
}

namespace coordinate
{
  struct absolute { static const bool is_absolute = true; };
  struct relative { static const bool is_absolute = false; };
}

namespace type
{
  struct number;
  struct length;
  struct integer;
  struct transform_list;
  struct path_data;
  struct number_optional_number;
  struct iri;
  struct funciri;
  struct color;
  struct color_optional_icc_color;
  struct angle;
  struct paint;
  struct percentage_or_length; // Length in CSS property can't be percentage itself
  struct clock_value;
  struct string;
  template<class Type> struct list_of;
  template<class LiteralsList> struct literal_enumeration;

#define SVGPP_TYPE_OR_LITERAL_ARITY 10
  template<class Type, class Value,
    BOOST_PP_ENUM_BINARY_PARAMS(SVGPP_TYPE_OR_LITERAL_ARITY, class Value, = boost::mpl::na BOOST_PP_INTERCEPT)> 
  struct type_or_literal;
}

// helper tags
struct min_tag {};
struct mid_tag {};
struct max_tag {};

namespace value 
{
#define SVGPP_ON_VALUE(tag) struct tag {};
#define SVGPP_ON_VALUE2(tag, string) SVGPP_ON_VALUE(tag)
#include <svgpp/detail/dict/enumerate_literal_values.inc>
#undef SVGPP_ON_VALUE
#undef SVGPP_ON_VALUE2

  struct rect                         {}; // Used as shape in 'clip' property
  struct underline                    {};
  struct overline                     {};
  struct line_through                 {};
  struct blink                        {};

  struct meet                         {};
  struct slice                        {};
  struct xMinYMin                     { typedef min_tag x; typedef min_tag y; };
  struct xMidYMin                     { typedef mid_tag x; typedef min_tag y; };
  struct xMaxYMin                     { typedef max_tag x; typedef min_tag y; };
  struct xMinYMid                     { typedef min_tag x; typedef mid_tag y; };
  struct xMidYMid                     { typedef mid_tag x; typedef mid_tag y; };
  struct xMaxYMid                     { typedef max_tag x; typedef mid_tag y; };
  struct xMinYMax                     { typedef min_tag x; typedef max_tag y; };
  struct xMidYMax                     { typedef mid_tag x; typedef max_tag y; };
  struct xMaxYMax                     { typedef max_tag x; typedef max_tag y; };
}

namespace length_dimension
{
  struct any { protected: any() {} };
  // Percentage length value should correspond to one of the following to be correctly converted to user coordinate system
  struct width                : any {};
  struct height               : any {};
  struct not_width_nor_height : any {};
}

struct iri_fragment {}; // Prefixes fragment part of IRI in arguments list

struct text_content; // Used in child element sequences together with tag::element::* tags to
                     // mark elements that should handle character data content

}} // namespace tag
