// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/traits/attribute_groups.hpp>
#include <svgpp/traits/element_groups.hpp>
#include <svgpp/traits/literal_enumeration_values.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/has_key.hpp>

namespace svgpp { namespace traits {

template<class Element, class Attribute, class Enable = void> 
struct attribute_type
{
  // By default type is specific to attribute
  typedef Attribute type;
};

template<class Element> struct attribute_type<Element, tag::attribute::x1               > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::y1               > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::x2               > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::y2               > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::width            > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::height           > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::r                > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::rx               > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::ry               > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::cx               > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::cy               > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::refX             > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::refY             > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::markerWidth      > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::markerHeight     > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::fx               > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::fy               > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::startOffset      > { typedef tag::type::length type; };
template<class Element> struct attribute_type<Element, tag::attribute::textLength       > { typedef tag::type::length type; };

template<class ElementTag, class AttributeTag>
struct attribute_type<ElementTag, AttributeTag,
  typename boost::enable_if_c<
    ( boost::mpl::has_key<filter_primitive_elements, ElementTag>::value
    || boost::mpl::has_key<boost::mpl::set9<
        tag::element::cursor, tag::element::filter, tag::element::foreignObject, tag::element::image,
        tag::element::pattern, tag::element::rect, tag::element::svg, tag::element::use_, 
        tag::element::mask>, ElementTag>::value )
    && boost::mpl::has_key<
      boost::mpl::set2<tag::attribute::x, tag::attribute::y>, AttributeTag>::value>::type>
{
  typedef tag::type::length type;
};

template<class Element> struct attribute_type<Element, tag::attribute::accent_height              > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::alphabetic                 > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::amplitude                  > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::ascent                     > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::azimuth                    > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::bias                       > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::cap_height                 > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::descent                    > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::diffuseConstant            > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::divisor                    > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::elevation                  > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::exponent                   > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::hanging                    > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::horiz_adv_x                > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::horiz_origin_x             > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::horiz_origin_y             > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::ideographic                > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::intercept                  > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::k                          > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::k1                         > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::k2                         > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::k3                         > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::k4                         > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::limitingConeAngle          > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::mathematical               > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::overline_position          > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::overline_thickness         > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::pathLength                 > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::pointsAtX                  > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::pointsAtY                  > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::pointsAtZ                  > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::scale                      > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::seed                       > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::slope                      > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::specularConstant           > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::specularExponent           > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::stemh                      > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::stemv                      > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::strikethrough_position     > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::strikethrough_thickness    > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::surfaceScale               > { typedef tag::type::number type; };  
template<class Element> struct attribute_type<Element, tag::attribute::underline_position         > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::underline_thickness        > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::units_per_em               > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::v_alphabetic               > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::v_hanging                  > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::v_ideographic              > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::v_mathematical             > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::version                    > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::vert_adv_y                 > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::vert_origin_x              > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::vert_origin_y              > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::x_height                   > { typedef tag::type::number type; };
template<class Element> struct attribute_type<Element, tag::attribute::z                          > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::feFuncA, tag::attribute::offset       > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::feFuncB, tag::attribute::offset       > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::feFuncG, tag::attribute::offset       > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::feFuncR, tag::attribute::offset       > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::feOffset, tag::attribute::dx          > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::feOffset, tag::attribute::dy          > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::glyphRef, tag::attribute::x           > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::glyphRef, tag::attribute::y           > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::glyphRef, tag::attribute::dx          > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::glyphRef, tag::attribute::dy          > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::fePointLight, tag::attribute::x       > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::fePointLight, tag::attribute::y       > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::feSpotLight, tag::attribute::x        > { typedef tag::type::number type; };
template<>              struct attribute_type<tag::element::feSpotLight, tag::attribute::y        > { typedef tag::type::number type; };

template<class Element> struct attribute_type<Element, tag::attribute::numOctaves>                { typedef tag::type::integer type; };
template<class Element> struct attribute_type<Element, tag::attribute::targetX   >                { typedef tag::type::integer type; }; 
template<class Element> struct attribute_type<Element, tag::attribute::targetY   >                { typedef tag::type::integer type; };

template<class Element> struct attribute_type<Element, tag::attribute::transform>                 { typedef tag::type::transform_list type; };
template<class Element> struct attribute_type<Element, tag::attribute::gradientTransform>         { typedef tag::type::transform_list type; };
template<class Element> struct attribute_type<Element, tag::attribute::patternTransform>          { typedef tag::type::transform_list type; };

template<class Element> struct attribute_type<Element, tag::attribute::d>                         { typedef tag::type::path_data type; };
template<>              struct attribute_type<tag::element::animateMotion, tag::attribute::path>  { typedef tag::type::path_data type; };

template<class ElementTag, class AttributeTag>
struct attribute_type<ElementTag, AttributeTag,
  typename boost::enable_if_c<
    boost::mpl::has_key<
      boost::mpl::set4<tag::element::altGlyph, tag::element::text, tag::element::tref, tag::element::tspan>, ElementTag>::value
    && boost::mpl::has_key<
      boost::mpl::set5<tag::attribute::x, tag::attribute::y, tag::attribute::dx, tag::attribute::dy, 
        tag::attribute::rotate>, AttributeTag>::value>::type>
{
  typedef tag::type::list_of<tag::type::length> type;
};

template<class Element> struct attribute_type<Element, tag::attribute::kernelMatrix>                { typedef tag::type::list_of<tag::type::number> type; };
template<class Element> struct attribute_type<Element, tag::attribute::keyPoints>                   { typedef tag::type::list_of<tag::type::number> type; };
template<class Element> struct attribute_type<Element, tag::attribute::tableValues>                 { typedef tag::type::list_of<tag::type::number> type; };
template<>              struct attribute_type<tag::element::feColorMatrix, tag::attribute::values>  { typedef tag::type::list_of<tag::type::number> type; };
template<>              struct attribute_type<tag::element::altGlyph, tag::attribute::rotate>       { typedef tag::type::list_of<tag::type::number> type; };
template<>              struct attribute_type<tag::element::text,     tag::attribute::rotate>       { typedef tag::type::list_of<tag::type::number> type; };
template<>              struct attribute_type<tag::element::tref,     tag::attribute::rotate>       { typedef tag::type::list_of<tag::type::number> type; };
template<>              struct attribute_type<tag::element::tspan,    tag::attribute::rotate>       { typedef tag::type::list_of<tag::type::number> type; };

template<class Element> struct attribute_type<Element, tag::attribute::baseFrequency>                     { typedef tag::type::number_optional_number type; };
template<class Element> struct attribute_type<Element, tag::attribute::kernelUnitLength>                  { typedef tag::type::number_optional_number type; };
template<>              struct attribute_type<tag::element::filter, tag::attribute::filterRes>            { typedef tag::type::number_optional_number type; };
template<>              struct attribute_type<tag::element::feConvolveMatrix, tag::attribute::order>      { typedef tag::type::number_optional_number type; };
template<>              struct attribute_type<tag::element::feMorphology, tag::attribute::radius>         { typedef tag::type::number_optional_number type; };
template<>              struct attribute_type<tag::element::feGaussianBlur, tag::attribute::stdDeviation> { typedef tag::type::number_optional_number type; };

template<class Element> struct attribute_type<Element, tag::attribute::fill_opacity>                    { typedef tag::type::type_or_literal<tag::type::number, tag::value::inherit> type; };
template<class Element> struct attribute_type<Element, tag::attribute::flood_opacity>                   { typedef tag::type::type_or_literal<tag::type::number, tag::value::inherit> type; };
template<class Element> struct attribute_type<Element, tag::attribute::opacity>                         { typedef tag::type::type_or_literal<tag::type::number, tag::value::inherit> type; };
template<class Element> struct attribute_type<Element, tag::attribute::stop_opacity>                    { typedef tag::type::type_or_literal<tag::type::number, tag::value::inherit> type; };
template<class Element> struct attribute_type<Element, tag::attribute::stroke_miterlimit>               { typedef tag::type::type_or_literal<tag::type::number, tag::value::inherit> type; };
template<class Element> struct attribute_type<Element, tag::attribute::stroke_opacity>                  { typedef tag::type::type_or_literal<tag::type::number, tag::value::inherit> type; };

namespace detail
{
  typedef tag::type::type_or_literal<tag::type::funciri, 
    tag::value::none,
    tag::value::inherit
  > funciri_or_none_or_inherit; 

  typedef tag::type::type_or_literal<tag::type::color_optional_icc_color, 
    tag::value::currentColor,
    tag::value::inherit
  > color_with_icc_or_current_or_inherit; 
}

template<class Element> struct attribute_type<Element, tag::attribute::clip_path>                       { typedef detail::funciri_or_none_or_inherit type; }; 
template<class Element> struct attribute_type<Element, tag::attribute::filter>                          { typedef detail::funciri_or_none_or_inherit type; };
template<class Element> struct attribute_type<Element, tag::attribute::marker>                          { typedef detail::funciri_or_none_or_inherit type; };
template<class Element> struct attribute_type<Element, tag::attribute::marker_end>                      { typedef detail::funciri_or_none_or_inherit type; };
template<class Element> struct attribute_type<Element, tag::attribute::marker_mid>                      { typedef detail::funciri_or_none_or_inherit type; };
template<class Element> struct attribute_type<Element, tag::attribute::marker_start>                    { typedef detail::funciri_or_none_or_inherit type; };
template<class Element> struct attribute_type<Element, tag::attribute::mask>                            { typedef detail::funciri_or_none_or_inherit type; };

template<class Element> struct attribute_type<Element, tag::attribute::flood_color>                     { typedef detail::color_with_icc_or_current_or_inherit type; }; 
template<class Element> struct attribute_type<Element, tag::attribute::lighting_color>                  { typedef detail::color_with_icc_or_current_or_inherit type; }; 
template<class Element> struct attribute_type<Element, tag::attribute::stop_color>                      { typedef detail::color_with_icc_or_current_or_inherit type; }; 

template<class Element> struct attribute_type<Element, tag::attribute::xlink::arcrole>                  { typedef tag::type::iri type; };
template<class Element> struct attribute_type<Element, tag::attribute::xlink::href>                     { typedef tag::type::iri type; };
template<class Element> struct attribute_type<Element, tag::attribute::xlink::role>                     { typedef tag::type::iri type; };
template<class Element> struct attribute_type<Element, tag::attribute::xml::base>                       { typedef tag::type::iri type; };

template<class Element> struct attribute_type<Element, tag::attribute::fill>   { typedef tag::type::paint type; };
template<class Element> struct attribute_type<Element, tag::attribute::stroke> { typedef tag::type::paint type; };

#define SVGPP_ON_ATTR(name, values) \
  template<class Element> struct attribute_type<Element, tag::attribute::name> \
    { typedef tag::type::literal_enumeration<literal_enumeration_values<tag::attribute::name>::type> type; };
#define SVGPP_ON_ATTR_NS(ns, name, values) SVGPP_ON_ATTR(ns::name, values)
#define SVGPP_ON_ELEM_ATTR(elem, attr, values) \
  template<> struct attribute_type<tag::element:: elem, tag::attribute:: attr> \
  { typedef tag::type::literal_enumeration<literal_enumeration_values<boost::mpl::pair<tag::element:: elem, tag::attribute:: attr> >::type> type; };
#define SVGPP_ON_ELEM_ATTR_NS(elem, ns, name, values) SVGPP_ON_ELEM_ATTR(elem, ns::name, values)
#include <svgpp/detail/dict/enumerate_literal_enumeration_attributes.inc>
#undef SVGPP_ON_ATTR
#undef SVGPP_ON_ATTR_NS
#undef SVGPP_ON_ELEM_ATTR
#undef SVGPP_ON_ELEM_ATTR_NS

template<> 
struct attribute_type<tag::element::stop, tag::attribute::offset> 
{
  typedef boost::mpl::pair<tag::element::stop, tag::attribute::offset> type;
};

template<class Element> struct attribute_type<Element, tag::attribute::font>                  
{ 
  typedef tag::type::type_or_literal<tag::attribute::font, 
    tag::value::inherit,
    tag::value::caption,
    tag::value::icon,
    tag::value::menu,
    tag::value::message_box,
    tag::value::small_caption,
    tag::value::status_bar
  > type; 
};

/*
'font-face' element attributes:
  font-size (Descriptor) Value:  	all | <length> [, <length>]*
  font-stretch = "all | [ normal | ultra-condensed | extra-condensed | condensed | semi-condensed | semi-expanded | expanded | extra-expanded | ultra-expanded] [, [ normal | ultra-condensed | extra-condensed | condensed | semi-condensed | semi-expanded | expanded | extra-expanded | ultra-expanded] ]*"
  font-style = "all | [ normal | italic | oblique] [, [normal | italic | oblique]]*"
  font-variant = "[normal | small-caps] [,[normal | small-caps]]*"
  font-weight = "all | [normal | bold | 100 | 200 | 300 | 400 | 500 | 600 | 700 | 800 | 900] [, [normal | bold | 100 | 200 | 300 | 400 | 500 | 600 | 700 | 800 | 900]]*"
g1 = "<name> [, <name> ]* "
g2 = "<name> [, <name> ]* "
u1 = "[<character> | <urange> ] [, [<character> | <urange>] ]* "
u2 = "[<character> | <urange> ] [, [<character> | <urange>] ]* "
widths (Descriptor) Value:  	[<urange> ]? [<number> ]+ [,[<urange> ]? <number> ]+]
unicode-range = "<urange> [, <urange>]*"
glyph-name = "<name> [, <name> ]* "
keySplines = "<list>"
keyTimes = "<list>"
panose-1 = "[<integer>]{10}"
values = "<list>" // 'animate', 'animateColor', 'animateMotion', 'animateTransform'
*/

template<class Element> struct attribute_type<Element, tag::attribute::baseline_shift> 
{ 
  typedef tag::type::type_or_literal<tag::type::percentage_or_length, // TODO: percentage is not relative to viewport!
    tag::value::baseline,
    tag::value::sub,
    tag::value::super,
    tag::value::inherit
  > type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::color> 
{ 
  typedef tag::type::type_or_literal<tag::type::color, tag::value::inherit> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::dur> 
{ 
  typedef tag::type::type_or_literal<tag::type::clock_value, tag::value::media, tag::value::indefinite> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::font_size> 
{ 
  // TODO: percent for font-size must be processed not as relative to viewport
  typedef tag::type::type_or_literal<tag::type::percentage_or_length, 
    tag::value::inherit,
    tag::value::xx_small,
    tag::value::x_small,
    tag::value::small,
    tag::value::medium,
    tag::value::large,
    tag::value::x_large,
    tag::value::xx_large,
    tag::value::larger,
    tag::value::smaller
  > type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::font_size_adjust> 
{ 
  typedef tag::type::type_or_literal<tag::type::number, 
    tag::value::none,
    tag::value::inherit
  > type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::glyph_orientation_horizontal> 
{ 
  typedef tag::type::type_or_literal<tag::type::angle, tag::value::inherit> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::glyph_orientation_vertical> 
{ 
  typedef tag::type::type_or_literal<tag::type::angle, tag::value::auto_, tag::value::inherit> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::in> 
{ 
  typedef tag::type::type_or_literal<tag::type::string, 
    tag::value::SourceGraphic,
    tag::value::SourceAlpha,
    tag::value::BackgroundImage,
    tag::value::BackgroundAlpha,
    tag::value::FillPaint,
    tag::value::StrokePaint
  > type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::in2> 
  : attribute_type<Element, tag::attribute::in> 
{};

template<class Element> struct attribute_type<Element, tag::attribute::kerning> 
{ 
  typedef tag::type::type_or_literal<tag::type::length, tag::value::auto_, tag::value::inherit> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::letter_spacing> 
{ 
  typedef tag::type::type_or_literal<tag::type::length, tag::value::normal, tag::value::inherit> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::max>
{ 
  typedef tag::type::type_or_literal<tag::type::clock_value, tag::value::media> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::min>
{ 
  typedef tag::type::type_or_literal<tag::type::clock_value, tag::value::media> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::orient> 
{ 
  typedef tag::type::type_or_literal<tag::type::angle, tag::value::auto_> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::repeatCount> 
{ 
  typedef tag::type::type_or_literal<tag::type::number, tag::value::indefinite> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::repeatDur> 
{ 
  typedef tag::type::type_or_literal<tag::type::clock_value, tag::value::indefinite> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::stroke_dasharray> 
{ 
  typedef tag::type::type_or_literal<
    tag::type::list_of<tag::type::percentage_or_length>, 
    tag::value::none, 
    tag::value::inherit
  > type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::stroke_dashoffset> 
{ 
  typedef tag::type::type_or_literal<tag::type::percentage_or_length, tag::value::inherit> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::stroke_width> 
{ 
  typedef tag::type::type_or_literal<tag::type::percentage_or_length, tag::value::inherit> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::word_spacing> 
{ 
  typedef tag::type::type_or_literal<tag::type::length, tag::value::normal, tag::value::inherit> type; 
};

template<> struct attribute_type<tag::element::animateMotion, tag::attribute::rotate> 
{ 
  typedef tag::type::type_or_literal<tag::type::number, tag::value::auto_, tag::value::auto_reverse> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::clip> 
{ 
  typedef tag::type::type_or_literal<tag::attribute::clip, tag::value::auto_, tag::value::inherit> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::color_profile> 
{ 
  // TODO: "auto | sRGB | <name> | <iri> | inherit" we should distinguish <iri> from <name>
  typedef tag::type::type_or_literal<tag::type::string, tag::value::auto_, tag::value::sRGB, tag::value::inherit> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::enable_background> 
{ 
  typedef tag::type::type_or_literal<tag::attribute::enable_background, tag::value::accumulate, tag::value::new_, 
    tag::value::inherit> type; 
};

template<class Element> struct attribute_type<Element, tag::attribute::text_decoration> 
{ 
  typedef tag::type::type_or_literal<tag::attribute::text_decoration, tag::value::none, tag::value::inherit> type; 
};

template<class ElementTag, class AttributeTag>
struct attribute_type<ElementTag, AttributeTag,
  typename boost::enable_if_c<
       boost::mpl::has_key<animation_event_attributes, AttributeTag>::value
    || boost::mpl::has_key<graphical_event_attributes, AttributeTag>::value
    || boost::mpl::has_key<document_event_attributes, AttributeTag>::value 
  >::type>
{
  typedef tag::type::string type;
};

template<class Element> struct attribute_type<Element, tag::attribute::baseProfile           > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::class_                > { typedef tag::type::string type; }; // TODO: parse list of strings
template<class Element> struct attribute_type<Element, tag::attribute::contentScriptType     > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::contentStyleType      > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::id                    > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::attributeName         > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::font_family           > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::format                > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::glyphRef              > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::lang                  > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::local                 > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::media                 > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::name                  > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::requiredExtensions    > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::requiredFeatures      > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::result                > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::string                > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::style                 > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::systemLanguage        > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::target                > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::title                 > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::unicode               > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::viewTarget            > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::begin                 > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::end                   > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::by                    > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::from                  > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::to                    > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::xlink::title          > { typedef tag::type::string type; };
template<class Element> struct attribute_type<Element, tag::attribute::xml::lang             > { typedef tag::type::string type; };
template<>              struct attribute_type<tag::element::script, tag::attribute::type     > { typedef tag::type::string type; };
template<>              struct attribute_type<tag::element::style, tag::attribute::type      > { typedef tag::type::string type; };

}}
