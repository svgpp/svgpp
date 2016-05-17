#include "common.hpp"

#include <svgpp/parser/external_function/parse_all_impl.hpp>
#include <svgpp/factory/color.hpp>

SVGPP_PARSE_PATH_DATA_IMPL(svg_string_t::value_type const *, number_t)
SVGPP_PARSE_TRANSFORM_IMPL(svg_string_t::value_type const *, number_t)
SVGPP_PARSE_PAINT_IMPL(svg_string_t::value_type const *, color_factory_t, svgpp::factory::icc_color::default_factory)
SVGPP_PARSE_COLOR_IMPL(svg_string_t::value_type const *, color_factory_t, svgpp::factory::icc_color::default_factory)
SVGPP_PARSE_PRESERVE_ASPECT_RATIO_IMPL(svg_string_t::value_type const *)
SVGPP_PARSE_MISC_IMPL(svg_string_t::value_type const *, number_t)
SVGPP_PARSE_CLIP_IMPL(svg_string_t::value_type const *, length_factory_t)
SVGPP_PARSE_LENGTH_IMPL(svg_string_t::value_type const *, length_factory_t)