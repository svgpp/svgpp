// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

// Properties
SVGPP_ON_ATTR(alignment_baseline,              (auto_)(baseline)(before_edge)(text_before_edge)(middle)(central)(after_edge)(text_after_edge)(ideographic)(alphabetic)(hanging)(mathematical)(inherit))
SVGPP_ON_ATTR(clip_rule,                       (nonzero)(evenodd)(inherit))  // Default: nonzero
SVGPP_ON_ATTR(color_interpolation,             (auto_)(sRGB)(linearRGB)(inherit))     // Default: sRGB
SVGPP_ON_ATTR(color_interpolation_filters,     (auto_)(sRGB)(linearRGB)(inherit))     // Default: linearRGB
SVGPP_ON_ATTR(color_rendering,                 (auto_)(optimizeSpeed)(optimizeQuality)(inherit))    // Default: auto
SVGPP_ON_ATTR(direction,                       (ltr)(rtl)(inherit))   // Default: ltr
SVGPP_ON_ATTR(display,                         (inline_)(block)(list_item)(run_in)(compact)(marker)(table)(inline_table)(table_row_group)(table_header_group)(table_footer_group)(table_row)(table_column_group)(table_column)(table_cell)(table_caption)(none)(inherit))   // Default: inline_
SVGPP_ON_ATTR(dominant_baseline,               (auto_)(use_script)(no_change)(reset_size)(ideographic)(alphabetic)(hanging)(mathematical)(central)(middle)(text_after_edge)(text_before_edge)(inherit))  // Default: auto_
SVGPP_ON_ATTR(fill_rule,                       (nonzero)(evenodd)(inherit))  // Default: nonzero
SVGPP_ON_ATTR(font_stretch,                    (normal)(wider)(narrower)(ultra_condensed)(extra_condensed)(condensed)(semi_condensed)(semi_expanded)(expanded)(extra_expanded)(ultra_expanded)(inherit)) // Default: normal
SVGPP_ON_ATTR(font_style,                      (normal)(italic)(oblique)(inherit)) // Default: normal
SVGPP_ON_ATTR(font_variant,                    (normal)(small_caps)(inherit)) // Default: normal
SVGPP_ON_ATTR(font_weight,                     (normal)(bold)(bolder)(lighter)(value_100)(value_200)(value_300)(value_400)(value_500)(value_600)(value_700)(value_800)(value_900)(inherit))  // Default: normal
SVGPP_ON_ATTR(image_rendering,                 (auto_)(optimizeSpeed)(optimizeQuality)(inherit))  // Default: auto
SVGPP_ON_ATTR(overflow,                        (visible)(hidden)(scroll)(auto_)(inherit))
SVGPP_ON_ATTR(pointer_events,                  (visiblePainted)(visibleFill)(visibleStroke)(visible)(painted)(fill)(stroke)(all)(none)(inherit))   // Default: visible
SVGPP_ON_ATTR(shape_rendering,                 (auto_)(optimizeSpeed)(crispEdges)(geometricPrecision)(inherit))  // Default: auto
SVGPP_ON_ATTR(stroke_linecap,                  (butt)(round)(square)(inherit)) // Default: butt
SVGPP_ON_ATTR(stroke_linejoin,                 (miter)(round)(bevel)(inherit)) // Default: miter
SVGPP_ON_ATTR(text_anchor,                     (start)(middle)(end)(inherit))  // Default: start
SVGPP_ON_ATTR(text_rendering,                  (auto_)(optimizeSpeed)(optimizeLegibility)(geometricPrecision)(inherit))  // Default: auto
SVGPP_ON_ATTR(unicode_bidi,                    (normal)(embed)(bidi_override)(inherit))   // Default: normal
SVGPP_ON_ATTR(visibility,                      (visible)(hidden)(collapse)(inherit))  // Default: visible
SVGPP_ON_ATTR(writing_mode,                    (lr_tb)(rl_tb)(tb_rl)(lr)(rl)(tb)(inherit)) // Default: lr_tb

// Attributes
SVGPP_ON_ATTR(accumulate,                      (none)(sum))
SVGPP_ON_ATTR(additive,                        (replace)(sum))
SVGPP_ON_ATTR(arabic_form,                     (initial)(medial)(terminal)(isolated))
SVGPP_ON_ATTR(attributeType,                   (CSS)(XML)(auto_))
SVGPP_ON_ATTR(calcMode,                        (discrete)(linear)(paced)(spline))
SVGPP_ON_ATTR(clipPathUnits,                   (userSpaceOnUse)(objectBoundingBox))
SVGPP_ON_ATTR(edgeMode,                        (duplicate)(wrap)(none))
SVGPP_ON_ATTR(externalResourcesRequired,       (false_)(true_))
SVGPP_ON_ELEM_ATTR(animate,           fill,    (freeze)(remove))
SVGPP_ON_ELEM_ATTR(animateColor,      fill,    (freeze)(remove))
SVGPP_ON_ELEM_ATTR(animateMotion,     fill,    (freeze)(remove))
SVGPP_ON_ELEM_ATTR(animateTransform,  fill,    (freeze)(remove))
SVGPP_ON_ELEM_ATTR(set,               fill,    (freeze)(remove))
SVGPP_ON_ATTR(filterUnits,                     (userSpaceOnUse)(objectBoundingBox))
SVGPP_ON_ATTR(gradientUnits,                   (userSpaceOnUse)(objectBoundingBox))
SVGPP_ON_ATTR(lengthAdjust,                    (spacing)(spacingAndGlyphs))
SVGPP_ON_ATTR(markerUnits,                     (strokeWidth)(userSpaceOnUse))
SVGPP_ON_ATTR(maskContentUnits,                (userSpaceOnUse)(objectBoundingBox))
SVGPP_ON_ATTR(maskUnits,                       (userSpaceOnUse)(objectBoundingBox))
SVGPP_ON_ATTR(method,                          (align)(stretch))
SVGPP_ON_ATTR(mode,                            (normal)(multiply)(screen)(darken)(lighten))
SVGPP_ON_ELEM_ATTR(feComposite, operator_,     (over)(in)(out)(atop)(xor_)(arithmetic))
SVGPP_ON_ELEM_ATTR(feMorphology, operator_,    (erode)(dilate))
SVGPP_ON_ATTR(orientation,                     (h)(v))
SVGPP_ON_ATTR(origin,                          (default_))
SVGPP_ON_ATTR(patternContentUnits,             (userSpaceOnUse)(objectBoundingBox))
SVGPP_ON_ATTR(patternUnits,                    (userSpaceOnUse)(objectBoundingBox))
SVGPP_ON_ATTR(preserveAlpha,                   (false_)(true_))
SVGPP_ON_ATTR(primitiveUnits,                  (userSpaceOnUse)(objectBoundingBox))
SVGPP_ON_ATTR(rendering_intent,                (auto_)(perceptual)(relative_colorimetric)(saturation)(absolute_colorimetric))
SVGPP_ON_ATTR(restart,                         (always)(whenNotActive)(never))
SVGPP_ON_ATTR(spacing,                         (auto_)(exact))
SVGPP_ON_ATTR(spreadMethod,                    (pad)(reflect)(repeat))
SVGPP_ON_ATTR(stitchTiles,                     (stitch)(noStitch))
SVGPP_ON_ELEM_ATTR(animateTransform, type,     (translate)(scale)(rotate)(skewX)(skewY))
SVGPP_ON_ELEM_ATTR(feColorMatrix, type,        (matrix)(saturate)(hueRotate)(luminanceToAlpha))
SVGPP_ON_ELEM_ATTR(feTurbulence, type,         (fractalNoise)(turbulence))
SVGPP_ON_ELEM_ATTR(feFuncA, type,              (identity)(table)(discrete)(linear)(gamma))
SVGPP_ON_ELEM_ATTR(feFuncB, type,              (identity)(table)(discrete)(linear)(gamma))
SVGPP_ON_ELEM_ATTR(feFuncG, type,              (identity)(table)(discrete)(linear)(gamma))
SVGPP_ON_ELEM_ATTR(feFuncR, type,              (identity)(table)(discrete)(linear)(gamma))
SVGPP_ON_ATTR(xChannelSelector,                (R)(G)(B)(A))
SVGPP_ON_ATTR(yChannelSelector,                (R)(G)(B)(A))
SVGPP_ON_ATTR(zoomAndPan,                      (disable)(magnify))

SVGPP_ON_ATTR_NS(xml,   space,                 (default_)(preserve))
SVGPP_ON_ATTR_NS(        xlink, actuate,       (onLoad))
SVGPP_ON_ELEM_ATTR_NS(a, xlink, actuate,       (onRequest))
SVGPP_ON_ATTR_NS(xlink, type,                  (simple))
SVGPP_ON_ATTR_NS(        xlink, show,          (new_)(replace)(embed)(other)(none))
SVGPP_ON_ELEM_ATTR_NS(a, xlink, show,          (new_)(replace)) 