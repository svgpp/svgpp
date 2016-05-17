#include <svgpp/parser/external_function/parse_all_impl.hpp>
#include <svgpp/factory/color.hpp>
#include "sample01j.hpp"

SVGPP_PARSE_PATH_DATA_IMPL(const char *, double)
SVGPP_PARSE_TRANSFORM_IMPL(const char *, double)
SVGPP_PARSE_PAINT_IMPL(const char *, ColorFactory, svgpp::factory::icc_color::default_factory)
SVGPP_PARSE_COLOR_IMPL(const char *, ColorFactory, svgpp::factory::icc_color::default_factory)
SVGPP_PARSE_PRESERVE_ASPECT_RATIO_IMPL(const char *)
SVGPP_PARSE_MISC_IMPL(const char *, double)