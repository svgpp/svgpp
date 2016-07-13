// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/has_key.hpp>
#include <boost/utility/enable_if.hpp>

namespace svgpp { namespace detail 
{

#define SVGPP_ON(attribute_name) \
  case attribute_id_## attribute_name: \
    fn(tag::attribute::attribute_name()); \
    break;

#define SVGPP_ON_NS(ns, attribute_name) \
  case attribute_id_ ## ns ## _ ## attribute_name: \
    fn(tag::attribute::ns::attribute_name()); \
    break;

template<class F>
inline bool css_id_to_tag(attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::a, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    SVGPP_ON(target)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::altGlyph, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(dx)
    SVGPP_ON(dy)
    SVGPP_ON(glyphRef)
    SVGPP_ON(rotate)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::altGlyphDef, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::altGlyphItem, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    default:
      return false;
  }
  return true;
}

template<class ElementTag, class F>
inline bool attribute_id_to_tag(ElementTag, attribute_id id, F & fn, 
  typename boost::enable_if_c<
     boost::is_same<ElementTag, tag::element::animate>::value
  || boost::is_same<ElementTag, tag::element::animateColor>::value>::type * = 0)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_attribute_target_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_timing_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_value_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_addition_attributes.inc>
    SVGPP_ON(externalResourcesRequired)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::animateMotion, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_timing_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_value_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_addition_attributes.inc>
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(path)
    SVGPP_ON(keyPoints)
    SVGPP_ON(rotate)
    SVGPP_ON(origin)
    SVGPP_ON(fill)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::animateTransform, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_attribute_target_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_timing_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_value_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_addition_attributes.inc>
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(type)
    SVGPP_ON(fill)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::circle, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    SVGPP_ON(cx)
    SVGPP_ON(cy)
    SVGPP_ON(r)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::clipPath, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    SVGPP_ON(clipPathUnits)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::color_profile, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(local)
    SVGPP_ON(name)
    SVGPP_ON(rendering_intent)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::cursor, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(x)
    SVGPP_ON(y)
    default:
      return false;
  }
  return true;
}

template<class ElementTag, class F>
inline bool attribute_id_to_tag(ElementTag, attribute_id id, F & fn, 
  typename boost::enable_if_c<
       boost::is_same<ElementTag, tag::element::defs>::value
    || boost::is_same<ElementTag, tag::element::g>::value>::type * = 0)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    default:
      return false;
  }
  return true;
}

template<class ElementTag, class F>
inline bool attribute_id_to_tag(ElementTag, attribute_id id, F & fn, 
  typename boost::enable_if_c<
       boost::is_same<ElementTag, tag::element::desc>::value
    || boost::is_same<ElementTag, tag::element::title>::value>::type * = 0)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::ellipse, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    SVGPP_ON(cx)
    SVGPP_ON(cy)
    SVGPP_ON(rx)
    SVGPP_ON(ry)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::feBlend, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(in)
    SVGPP_ON(in2)
    SVGPP_ON(mode)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::feColorMatrix, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(in)
    SVGPP_ON(type)
    SVGPP_ON(values)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::feComponentTransfer, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(in)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::feComposite, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(in)
    SVGPP_ON(in2)
    SVGPP_ON(operator_)
    SVGPP_ON(k1)
    SVGPP_ON(k2)
    SVGPP_ON(k3)
    SVGPP_ON(k4)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::feConvolveMatrix, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(in)
    SVGPP_ON(order)
    SVGPP_ON(kernelMatrix)
    SVGPP_ON(divisor)
    SVGPP_ON(bias)
    SVGPP_ON(targetX)
    SVGPP_ON(targetY)
    SVGPP_ON(edgeMode)
    SVGPP_ON(kernelUnitLength)
    SVGPP_ON(preserveAlpha)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::feDiffuseLighting, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(in)
    SVGPP_ON(surfaceScale)
    SVGPP_ON(diffuseConstant)
    SVGPP_ON(kernelUnitLength)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::feDisplacementMap, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(in)
    SVGPP_ON(in2)
    SVGPP_ON(scale)
    SVGPP_ON(xChannelSelector)
    SVGPP_ON(yChannelSelector)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::feDistantLight, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    SVGPP_ON(azimuth)
    SVGPP_ON(elevation)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::feFlood, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    default:
      return false;
  }
  return true;
}

template<class ElementTag, class F>
inline bool attribute_id_to_tag(ElementTag, attribute_id id, F & fn, 
  typename boost::enable_if<boost::mpl::has_key<boost::mpl::set4<
    tag::element::feFuncA, tag::element::feFuncB, tag::element::feFuncR, tag::element::feFuncG>::type, ElementTag> >::type * = 0)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_transfer_function_element_attributes.inc>
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::feGaussianBlur, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(in)
    SVGPP_ON(stdDeviation)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::feImage, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(preserveAspectRatio)
    default:
      return false;
  }
  return true;
} 

template<class F>
inline bool attribute_id_to_tag(tag::element::feMerge, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    default:
      return false;
  }
  return true;
} 

template<class F>
inline bool attribute_id_to_tag(tag::element::feMergeNode, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    SVGPP_ON(in)
    default:
      return false;
  }
  return true;
} 

template<class F>
inline bool attribute_id_to_tag(tag::element::feMorphology, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(in)
    SVGPP_ON(operator_)
    SVGPP_ON(radius)
    default:
      return false;
  }
  return true;
} 

template<class F>
inline bool attribute_id_to_tag(tag::element::feOffset, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(in)
    SVGPP_ON(dx)
    SVGPP_ON(dy)
    default:
      return false;
  }
  return true;
} 

template<class F>
inline bool attribute_id_to_tag(tag::element::fePointLight, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(z)
    default:
      return false;
  }
  return true;
} 

template<class F>
inline bool attribute_id_to_tag(tag::element::feSpecularLighting, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(in)
    SVGPP_ON(surfaceScale)
    SVGPP_ON(specularConstant)
    SVGPP_ON(specularExponent)
    SVGPP_ON(kernelUnitLength)
    default:
      return false;
  }
  return true;
} 

template<class F>
inline bool attribute_id_to_tag(tag::element::feSpotLight, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(z)
    SVGPP_ON(pointsAtX)
    SVGPP_ON(pointsAtY)
    SVGPP_ON(pointsAtZ)
    SVGPP_ON(specularExponent)
    SVGPP_ON(limitingConeAngle)
    default:
      return false;
  }
  return true;
} 

template<class F>
inline bool attribute_id_to_tag(tag::element::feTile, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(in)
    default:
      return false;
  }
  return true;
} 

template<class F>
inline bool attribute_id_to_tag(tag::element::feTurbulence, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_filter_primitive_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(baseFrequency)
    SVGPP_ON(numOctaves)
    SVGPP_ON(seed)
    SVGPP_ON(stitchTiles)
    SVGPP_ON(type)
    default:
      return false;
  }
  return true;
} 

template<class F>
inline bool attribute_id_to_tag(tag::element::filter, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(width)
    SVGPP_ON(height)
    SVGPP_ON(filterRes)
    SVGPP_ON(filterUnits)
    SVGPP_ON(primitiveUnits)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::font, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(horiz_origin_x)
    SVGPP_ON(horiz_origin_y)
    SVGPP_ON(horiz_adv_x)
    SVGPP_ON(vert_origin_x)
    SVGPP_ON(vert_origin_y)
    SVGPP_ON(vert_adv_y)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::font_face, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    SVGPP_ON(font_family)
    SVGPP_ON(font_style)
    SVGPP_ON(font_variant)
    SVGPP_ON(font_weight)
    SVGPP_ON(font_stretch)
    SVGPP_ON(font_size)
    SVGPP_ON(unicode_range)
    SVGPP_ON(units_per_em)
    SVGPP_ON(panose_1)
    SVGPP_ON(stemv)
    SVGPP_ON(stemh)
    SVGPP_ON(slope)
    SVGPP_ON(cap_height)
    SVGPP_ON(x_height)
    SVGPP_ON(accent_height)
    SVGPP_ON(ascent)
    SVGPP_ON(descent)
    SVGPP_ON(widths)
    SVGPP_ON(bbox)
    SVGPP_ON(ideographic)
    SVGPP_ON(alphabetic)
    SVGPP_ON(mathematical)
    SVGPP_ON(hanging)
    SVGPP_ON(v_ideographic)
    SVGPP_ON(v_alphabetic)
    SVGPP_ON(v_mathematical)
    SVGPP_ON(v_hanging)
    SVGPP_ON(underline_position)
    SVGPP_ON(underline_thickness)
    SVGPP_ON(strikethrough_position)
    SVGPP_ON(strikethrough_thickness)
    SVGPP_ON(overline_position)
    SVGPP_ON(overline_thickness)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::font_face_format, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    SVGPP_ON(string)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::font_face_name, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    SVGPP_ON(string)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::font_face_src, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::font_face_uri, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::foreignObject, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(width)
    SVGPP_ON(height)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::glyph, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(d)
    SVGPP_ON(horiz_adv_x)
    SVGPP_ON(vert_origin_x)
    SVGPP_ON(vert_origin_y)
    SVGPP_ON(vert_adv_y)
    SVGPP_ON(unicode)
    SVGPP_ON(glyph_name)
    SVGPP_ON(orientation)
    SVGPP_ON(arabic_form)
    SVGPP_ON(lang)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::glyphRef, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(dx)
    SVGPP_ON(dy)
    SVGPP_ON(glyphRef)
    SVGPP_ON(format)
    default:
      return false;
  }
  return true;
}


template<class F>
inline bool attribute_id_to_tag(tag::element::hkern, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    SVGPP_ON(u1)
    SVGPP_ON(g1)
    SVGPP_ON(u2)
    SVGPP_ON(g2)
    SVGPP_ON(k)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::image, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(preserveAspectRatio)
    SVGPP_ON(transform)
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(width)
    SVGPP_ON(height)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::line, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    SVGPP_ON(x1)
    SVGPP_ON(y1)
    SVGPP_ON(x2)
    SVGPP_ON(y2)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::linearGradient, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(x1)
    SVGPP_ON(y1)
    SVGPP_ON(x2)
    SVGPP_ON(y2)
    SVGPP_ON(gradientUnits)
    SVGPP_ON(gradientTransform)
    SVGPP_ON(spreadMethod)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::marker, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(viewBox)
    SVGPP_ON(preserveAspectRatio)
    SVGPP_ON(refX)
    SVGPP_ON(refY)
    SVGPP_ON(markerUnits)
    SVGPP_ON(markerWidth)
    SVGPP_ON(markerHeight)
    SVGPP_ON(orient)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::mask, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(width)
    SVGPP_ON(height)
    SVGPP_ON(maskUnits)
    SVGPP_ON(maskContentUnits)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::metadata, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::missing_glyph, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(d)
    SVGPP_ON(horiz_adv_x)
    SVGPP_ON(vert_origin_x)
    SVGPP_ON(vert_origin_y)
    SVGPP_ON(vert_adv_y)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::mpath, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(externalResourcesRequired)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::path, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    SVGPP_ON(d)
    SVGPP_ON(pathLength)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::pattern, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(viewBox)
    SVGPP_ON(preserveAspectRatio)
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(width)
    SVGPP_ON(height)
    SVGPP_ON(patternUnits)
    SVGPP_ON(patternContentUnits)
    SVGPP_ON(patternTransform)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::polygon, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    SVGPP_ON(points)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::polyline, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    SVGPP_ON(points)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::radialGradient, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(cx)
    SVGPP_ON(cy)
    SVGPP_ON(r)
    SVGPP_ON(fx)
    SVGPP_ON(fy)
    SVGPP_ON(gradientUnits)
    SVGPP_ON(gradientTransform)
    SVGPP_ON(spreadMethod)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::rect, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(width)
    SVGPP_ON(height)
    SVGPP_ON(rx)
    SVGPP_ON(ry)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::script, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(type)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::set, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_attribute_target_attributes.inc>
#include <svgpp/detail/dict/enumerate_animation_timing_attributes.inc>
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(to)
    SVGPP_ON(fill)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::stop, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(offset)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::style, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    SVGPP_ON(type)
    SVGPP_ON(media)
    SVGPP_ON(title)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::svg, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_document_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(width)
    SVGPP_ON(height)
    SVGPP_ON(viewBox)
    SVGPP_ON(preserveAspectRatio)
    SVGPP_ON(zoomAndPan)
    SVGPP_ON(version)
    SVGPP_ON(baseProfile)
    SVGPP_ON(contentScriptType)
    SVGPP_ON(contentStyleType)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::switch_, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::symbol, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(viewBox)
    SVGPP_ON(preserveAspectRatio)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::text, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    SVGPP_ON(lengthAdjust)
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(dx)
    SVGPP_ON(dy)
    SVGPP_ON(rotate)
    SVGPP_ON(textLength)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::textPath, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(startOffset)
    SVGPP_ON(method)
    SVGPP_ON(spacing)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::tref, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::tspan, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(dx)
    SVGPP_ON(dy)
    SVGPP_ON(rotate)
    SVGPP_ON(textLength)
    SVGPP_ON(lengthAdjust)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::use_, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_conditional_processing_attributes.inc>
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
#include <svgpp/detail/dict/enumerate_graphical_event_attributes.inc>
#include <svgpp/detail/dict/enumerate_presentation_attributes.inc>
#include <svgpp/detail/dict/enumerate_xlink_attributes.inc>
    SVGPP_ON(class_)
    SVGPP_ON(style)
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(transform)
    SVGPP_ON(x)
    SVGPP_ON(y)
    SVGPP_ON(width)
    SVGPP_ON(height)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::view, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    SVGPP_ON(externalResourcesRequired)
    SVGPP_ON(viewBox)
    SVGPP_ON(preserveAspectRatio)
    SVGPP_ON(zoomAndPan)
    SVGPP_ON(viewTarget)
    default:
      return false;
  }
  return true;
}

template<class F>
inline bool attribute_id_to_tag(tag::element::vkern, attribute_id id, F & fn)
{
  switch (id)
  {
#include <svgpp/detail/dict/enumerate_core_attributes.inc>
    SVGPP_ON(u1)
    SVGPP_ON(g1)
    SVGPP_ON(u2)
    SVGPP_ON(g2)
    SVGPP_ON(k)
    default:
      return false;
  }
  return true;
}

#undef SVGPP_ON  
#undef SVGPP_ON_NS

}}