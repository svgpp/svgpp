// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/traits/element_groups.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/has_key.hpp>
#include <boost/mpl/empty_sequence.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/utility/enable_if.hpp>

namespace svgpp { namespace traits
{

template<class ElementTag, class Enable = void>
struct child_element_types;

template<class ElementTag>
struct child_element_types<ElementTag, typename boost::enable_if<boost::mpl::has_key<boost::mpl::set10<
  tag::element::foreignObject, tag::element::glyphRef, tag::element::hkern, tag::element::font_face_format,
  tag::element::font_face_name, tag::element::vkern,
  tag::element::feFuncA, tag::element::feFuncR, tag::element::feFuncG, tag::element::feFuncB
>, ElementTag> >::type>
{
  typedef boost::mpl::set0<> type;
};

template<class ElementTag>
struct child_element_types<ElementTag, typename boost::enable_if<boost::mpl::has_key<boost::mpl::set6<
  tag::element::altGlyph, tag::element::desc, tag::element::metadata, 
  tag::element::script, tag::element::style, tag::element::title
>, ElementTag> >::type>
{
  typedef boost::mpl::set1<tag::text_content> type;
};

template<class ElementTag>
struct child_element_types<ElementTag, typename boost::enable_if<boost::mpl::has_key<boost::mpl::set10<
  tag::element::svg, tag::element::g, tag::element::glyph, tag::element::a, tag::element::defs, tag::element::symbol,
  tag::element::marker, tag::element::mask, tag::element::missing_glyph, tag::element::pattern>, ElementTag> >::type>
{
  typedef 
    boost::mpl::fold<
      boost::mpl::joint_view<
      boost::mpl::joint_view<
      boost::mpl::joint_view<
      boost::mpl::joint_view<
        traits::animation_elements,
        traits::descriptive_elements>,
        traits::shape_elements>,
        traits::structural_elements>,
        traits::gradient_elements
      >,
      boost::mpl::set18<
        tag::element::a,
        tag::element::altGlyphDef,
        tag::element::clipPath,
        tag::element::color_profile,
        tag::element::cursor,
        tag::element::filter,
        tag::element::font,
        tag::element::font_face,
        tag::element::foreignObject,
        tag::element::image,
        tag::element::marker,
        tag::element::mask,
        tag::element::pattern,
        tag::element::script,
        tag::element::style,
        tag::element::switch_,
        tag::element::text,
        tag::element::view
      >,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<class ElementTag>
struct child_element_types<ElementTag, typename boost::enable_if<boost::mpl::has_key<boost::mpl::set9<
  tag::element::rect, tag::element::circle, tag::element::ellipse, tag::element::line, 
  tag::element::polygon, tag::element::polyline, tag::element::path, tag::element::image,
  tag::element::use_>, ElementTag> >::type>
{
  typedef 
    boost::mpl::fold<
      traits::animation_elements,
      traits::descriptive_elements,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<>
struct child_element_types<tag::element::text, void>
{
  typedef 
    boost::mpl::fold<
      boost::mpl::joint_view<
        traits::animation_elements,
        traits::descriptive_elements>,
      boost::mpl::set6<
        tag::element::altGlyph,
        tag::element::textPath,
        tag::element::tref,
        tag::element::tspan,
        tag::element::a,
        tag::text_content
      >,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<class ElementTag>
struct child_element_types<ElementTag, typename boost::enable_if<boost::mpl::has_key<boost::mpl::set2<
  tag::element::textPath, tag::element::tspan>, ElementTag> >::type>
{
  typedef 
    boost::mpl::fold<
      traits::descriptive_elements,
      boost::mpl::set8<
        tag::element::a,
        tag::element::altGlyph,
        tag::element::animate,
        tag::element::animateColor,
        tag::element::set,
        tag::element::tref,
        tag::element::tspan,
        tag::text_content
      >,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<>
struct child_element_types<tag::element::tref, void>
{
  typedef 
    boost::mpl::fold<
      traits::descriptive_elements,
      boost::mpl::set3<
        tag::element::animate,
        tag::element::animateColor,
        tag::element::set
      >,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<>
struct child_element_types<tag::element::clipPath, void>
{
  typedef
    boost::mpl::fold<
      boost::mpl::joint_view<
      boost::mpl::joint_view<
        traits::animation_elements,
        traits::descriptive_elements>,
        traits::shape_elements>,
      boost::mpl::set2<
        tag::element::text,
        tag::element::use_
      >,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<class ElementTag>
struct child_element_types<ElementTag, typename boost::enable_if<boost::mpl::has_key<boost::mpl::set8<
  tag::element::animate, tag::element::animateColor, tag::element::animateTransform, 
  tag::element::color_profile, tag::element::cursor, tag::element::mpath, tag::element::set,
  tag::element::view>, ElementTag> >::type>
{
  typedef traits::descriptive_elements type;
};

template<>
struct child_element_types<tag::element::altGlyphDef, void>
{
  typedef
    boost::mpl::set2<
      tag::element::altGlyphItem,
      tag::element::glyphRef
    >
    type;
};

template<>
struct child_element_types<tag::element::altGlyphItem, void>
{
  typedef
    boost::mpl::set1<
      tag::element::glyphRef
    >
    type;
};

template<>
struct child_element_types<tag::element::animateMotion, void>
{
  typedef
    boost::mpl::fold<
      traits::descriptive_elements,
      boost::mpl::set1<
        tag::element::mpath
      >,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<>
struct child_element_types<tag::element::filter, void>
{
  typedef
    boost::mpl::fold<
      boost::mpl::joint_view<
        traits::filter_primitive_elements,
        traits::descriptive_elements>,
      boost::mpl::set2<
        tag::element::animate,
        tag::element::set
      >,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<>
struct child_element_types<tag::element::font, void>
{
  typedef 
    boost::mpl::fold<
      traits::descriptive_elements,
      boost::mpl::set5<
        tag::element::font_face,
        tag::element::glyph,
        tag::element::hkern,
        tag::element::missing_glyph,
        tag::element::vkern
      >,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<>
struct child_element_types<tag::element::font_face, void>
{
  typedef 
    boost::mpl::fold<
      traits::descriptive_elements,
      boost::mpl::set1<
        tag::element::font_face_src
      >,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<>
struct child_element_types<tag::element::font_face_uri, void>
{
  typedef boost::mpl::set1<tag::element::font_face_format> type;
};

template<>
struct child_element_types<tag::element::font_face_src, void>
{
  typedef boost::mpl::set2<
    tag::element::font_face_name,
    tag::element::font_face_uri
  > type;
};

template<class ElementTag>
struct child_element_types<ElementTag, typename boost::enable_if<boost::mpl::has_key<boost::mpl::set2<
  tag::element::linearGradient, tag::element::radialGradient>, ElementTag> >::type>
{
  typedef 
    boost::mpl::fold<
      traits::descriptive_elements,
      boost::mpl::set4<
        tag::element::animate,
        tag::element::animateTransform,
        tag::element::set,
        tag::element::stop
      >,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<>
struct child_element_types<tag::element::stop, void>
{
  typedef 
    boost::mpl::set3<
      tag::element::animate,
      tag::element::animateColor,
      tag::element::set
    > type;
};

template<>
struct child_element_types<tag::element::switch_, void>
{
  typedef 
    boost::mpl::fold<
      boost::mpl::joint_view<
      boost::mpl::joint_view<
        traits::animation_elements,
        traits::descriptive_elements>,
        traits::shape_elements>,
      boost::mpl::set8<
        tag::element::a,
        tag::element::foreignObject,
        tag::element::g,
        tag::element::image,
        tag::element::svg,
        tag::element::switch_,
        tag::element::text,
        tag::element::use_
      >,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<class ElementTag>
struct child_element_types<ElementTag, typename boost::enable_if<boost::mpl::has_key<boost::mpl::set14<
  tag::element::feBlend, tag::element::feColorMatrix, tag::element::feComposite, tag::element::feConvolveMatrix, 
  tag::element::feDisplacementMap, tag::element::feDistantLight, tag::element::feGaussianBlur, tag::element::feMergeNode,
  tag::element::feMorphology, tag::element::feOffset, tag::element::fePointLight, tag::element::feSpotLight,
  tag::element::feTile, tag::element::feTurbulence>, ElementTag> >::type>
{
  typedef 
    boost::mpl::set2<
      tag::element::animate,
      tag::element::set
    > type;
};

template<>
struct child_element_types<tag::element::feComponentTransfer, void>
{
  typedef 
    boost::mpl::set4<
      svgpp::tag::element::feFuncA,
      svgpp::tag::element::feFuncB,
      svgpp::tag::element::feFuncG,
      svgpp::tag::element::feFuncR
    > type;
};

template<>
struct child_element_types<tag::element::feDiffuseLighting, void>
{
  typedef 
    boost::mpl::fold<
      traits::light_source_elements,
      traits::descriptive_elements,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<>
struct child_element_types<tag::element::feSpecularLighting, void>
{
  typedef 
    boost::mpl::fold<
      traits::light_source_elements,
      traits::descriptive_elements,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type type;
};

template<>
struct child_element_types<tag::element::feFlood, void>
{
  typedef 
    boost::mpl::set3<
      tag::element::animate,
      tag::element::animateColor,
      tag::element::set
    > type;
};

template<>
struct child_element_types<tag::element::feImage, void>
{
  typedef 
    boost::mpl::set3<
      tag::element::animate,
      tag::element::animateTransform,
      tag::element::set
    > type;
};

template<>
struct child_element_types<tag::element::feMerge, void>
{
  typedef 
    boost::mpl::set1<
      tag::element::feMergeNode
    > type;
};

}}
