// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/joint_view.hpp>

namespace svgpp { namespace traits {

template<class AttributeTag>
struct is_presentation_attribute: 
  boost::mpl::bool_<AttributeTag::attribute_id <= ::svgpp::detail::last_styling_attribute> {};

typedef boost::mpl::set8<
  tag::attribute::font_family, 
  tag::attribute::font_style, 
  tag::attribute::font_variant, 
  tag::attribute::font_weight, 
  tag::attribute::font_stretch, 
  tag::attribute::font_size, 
  tag::attribute::font_size_adjust,
  tag::attribute::font
  > font_selection_attributes;

typedef boost::mpl::set6<
  tag::attribute::x, 
  tag::attribute::y, 
  tag::attribute::width, 
  tag::attribute::height, 
  tag::attribute::viewBox, 
  tag::attribute::preserveAspectRatio
  > viewport_attributes;

typedef boost::mpl::set6<
  tag::attribute::refX, 
  tag::attribute::refY, 
  tag::attribute::markerWidth, 
  tag::attribute::markerHeight, 
  tag::attribute::viewBox, 
  tag::attribute::preserveAspectRatio
  > marker_viewport_attributes;

typedef boost::mpl::set3<
  tag::attribute::requiredFeatures, 
  tag::attribute::requiredExtensions, 
  tag::attribute::systemLanguage
  > conditional_processing_attributes;

typedef boost::mpl::set10<
  tag::attribute::onfocusin, 
  tag::attribute::onfocusout, 
  tag::attribute::onactivate,
  tag::attribute::onclick, 
  tag::attribute::onmousedown, 
  tag::attribute::onmouseup,
  tag::attribute::onmouseover, 
  tag::attribute::onmousemove, 
  tag::attribute::onmouseout,
  tag::attribute::onload
  > graphical_event_attributes;

typedef boost::mpl::set6<
  tag::attribute::onunload, 
  tag::attribute::onabort, 
  tag::attribute::onerror, 
  tag::attribute::onresize, 
  tag::attribute::onscroll, 
  tag::attribute::onzoom
  > document_event_attributes;

typedef boost::mpl::set3<
  tag::attribute::onbegin, 
  tag::attribute::onend, 
  tag::attribute::onrepeat
  > animation_event_attributes;

typedef boost::mpl::set<
  boost::mpl::pair<tag::element::path, tag::attribute::d>,
  boost::mpl::pair<tag::element::rect, tag::attribute::x>,
  boost::mpl::pair<tag::element::rect, tag::attribute::y>,
  boost::mpl::pair<tag::element::rect, tag::attribute::width>, 
  boost::mpl::pair<tag::element::rect, tag::attribute::height>,
  boost::mpl::pair<tag::element::rect, tag::attribute::rx>,
  boost::mpl::pair<tag::element::rect, tag::attribute::ry>,
  boost::mpl::pair<tag::element::circle, tag::attribute::cx>,
  boost::mpl::pair<tag::element::circle, tag::attribute::cy>,
  boost::mpl::pair<tag::element::circle, tag::attribute::r>,
  boost::mpl::pair<tag::element::ellipse, tag::attribute::cx>,
  boost::mpl::pair<tag::element::ellipse, tag::attribute::cy>,
  boost::mpl::pair<tag::element::ellipse, tag::attribute::rx>,
  boost::mpl::pair<tag::element::ellipse, tag::attribute::ry>,
  boost::mpl::pair<tag::element::line, tag::attribute::x1>,
  boost::mpl::pair<tag::element::line, tag::attribute::y1>,
  boost::mpl::pair<tag::element::line, tag::attribute::x2>,
  boost::mpl::pair<tag::element::line, tag::attribute::y2>,
  boost::mpl::pair<tag::element::polyline, tag::attribute::points>,
  boost::mpl::pair<tag::element::polygon, tag::attribute::points>
  >::type shapes_attributes_by_element;

}}