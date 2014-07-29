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

// TODO: use single sequence
typedef 
  boost::mpl::joint_view<
  boost::mpl::vector<
    tag::attribute::alignment_baseline,            
    tag::attribute::baseline_shift,
    tag::attribute::clip_path, 
    tag::attribute::clip_rule,
    tag::attribute::clip,
    tag::attribute::color_interpolation_filters,
    tag::attribute::color_interpolation,
    tag::attribute::color_profile,
    tag::attribute::color_rendering,
    tag::attribute::color>,
  boost::mpl::joint_view<
  boost::mpl::vector<
    tag::attribute::cursor,
    tag::attribute::direction,
    tag::attribute::display,
    tag::attribute::dominant_baseline,
    tag::attribute::enable_background,
    tag::attribute::fill_opacity,
    tag::attribute::fill_rule,
    tag::attribute::fill,
    tag::attribute::filter,
    tag::attribute::flood_color>,
  boost::mpl::joint_view<
  boost::mpl::vector<
    tag::attribute::flood_opacity,
    tag::attribute::font_family,
    tag::attribute::font_size_adjust,
    tag::attribute::font_size,
    tag::attribute::font_stretch,
    tag::attribute::font_style,
    tag::attribute::font_variant,
    tag::attribute::font_weight,
    tag::attribute::glyph_orientation_horizontal,
    tag::attribute::glyph_orientation_vertical>,
  boost::mpl::joint_view<
  boost::mpl::vector<
    tag::attribute::image_rendering, 
    tag::attribute::kerning, 
    tag::attribute::letter_spacing, 
    tag::attribute::lighting_color, 
    tag::attribute::marker_end, 
    tag::attribute::marker_mid, 
    tag::attribute::marker_start, 
    tag::attribute::mask, 
    tag::attribute::opacity, 
    tag::attribute::overflow>, 
  boost::mpl::joint_view<
  boost::mpl::vector<
    tag::attribute::pointer_events, 
    tag::attribute::shape_rendering, 
    tag::attribute::stop_color, 
    tag::attribute::stop_opacity, 
    tag::attribute::stroke_dasharray, 
    tag::attribute::stroke_dashoffset, 
    tag::attribute::stroke_linecap, 
    tag::attribute::stroke_linejoin, 
    tag::attribute::stroke_miterlimit, 
    tag::attribute::stroke_opacity>, 
  boost::mpl::vector<
    tag::attribute::stroke_width, 
    tag::attribute::stroke, 
    tag::attribute::text_anchor, 
    tag::attribute::text_decoration, 
    tag::attribute::text_rendering, 
    tag::attribute::unicode_bidi, 
    tag::attribute::visibility, 
    tag::attribute::word_spacing,
    tag::attribute::writing_mode,
    tag::attribute::font>
  > > > > > presentation_attributes;

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