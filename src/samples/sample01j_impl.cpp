#include <svgpp/parser/external_function/parse_path_data_impl.hpp>
#include <svgpp/parser/external_function/parse_transform_impl.hpp>
#include <svgpp/parser/external_function/parse_paint_impl.hpp>
#include <svgpp/parser/external_function/parse_preserveAspectRatio_impl.hpp>
#include <svgpp/parser/external_function/parse_misc_impl.hpp>
#include <svgpp/factory/color.hpp>
#include "sample01j.hpp"

SVGPP_PARSE_PATH_DATA_IMPL(const char *, double)
SVGPP_PARSE_TRANSFORM_IMPL(const char *, double)
typedef svgpp::factory::icc_color::stub const default_icc_factory_t;
SVGPP_PARSE_PAINT_IMPL(const char *, ColorFactory, default_icc_factory_t)
SVGPP_PARSE_PRESERVE_ASPECT_RATIO_IMPL(const char *)
SVGPP_PARSE_MISC_IMPL(const char *, double)