// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/set.hpp>

namespace svgpp { namespace traits
{

typedef boost::mpl::set5<
  tag::element::animate,
  tag::element::animateColor,
  tag::element::animateMotion,
  tag::element::animateTransform,
  tag::element::set
> animation_elements;

typedef boost::mpl::set3<
  tag::element::desc,
  tag::element::metadata,
  tag::element::title
> descriptive_elements;

typedef boost::mpl::set7<
  tag::element::circle,
  tag::element::ellipse,
  tag::element::line,
  tag::element::path,
  tag::element::polygon,
  tag::element::polyline,
  tag::element::rect
> shape_elements;

typedef boost::mpl::fold<
  boost::mpl::vector3<
    tag::element::image,
    tag::element::text,
    tag::element::use_
  >,
  shape_elements,
  boost::mpl::insert<boost::mpl::_1,boost::mpl::_2>
>::type graphics_elements;

typedef boost::mpl::set5<
  tag::element::defs,
  tag::element::g,
  tag::element::svg,
  tag::element::symbol,
  tag::element::use_
> structural_elements;

typedef boost::mpl::set2<
  tag::element::linearGradient,
  tag::element::radialGradient
> gradient_elements;

typedef boost::mpl::set16<
  tag::element::feBlend,
  tag::element::feColorMatrix,
  tag::element::feComponentTransfer,
  tag::element::feComposite,
  tag::element::feConvolveMatrix,
  tag::element::feDiffuseLighting,
  tag::element::feDisplacementMap,
  tag::element::feFlood,
  tag::element::feGaussianBlur,
  tag::element::feImage,
  tag::element::feMerge,
  tag::element::feMorphology,
  tag::element::feOffset,
  tag::element::feSpecularLighting,
  tag::element::feTile,
  tag::element::feTurbulence
> filter_primitive_elements;

typedef boost::mpl::set3<
  tag::element::feDistantLight,
  tag::element::fePointLight,
  tag::element::feSpotLight
> light_source_elements;

typedef boost::mpl::fold<
  boost::mpl::vector3<
    tag::element::svg,
    tag::element::g,
    tag::element::symbol
  >,
  graphics_elements,
  boost::mpl::insert<boost::mpl::_1,boost::mpl::_2>
>::type reusable_elements; // Can be referenced by 'use' element

}}
