#pragma once

#define BOOST_PARAMETER_MAX_ARITY 15

#include <agg_color_rgba.h>
#include <svgpp/factory/integer_color.hpp>
#include <svgpp/factory/unitless_length.hpp>
#ifdef SVG_PARSER_MSXML
# include "parser_msxml.hpp"
#elif defined(SVG_PARSER_RAPIDXML_NS)
# include "parser_rapidxml_ns.hpp"
#elif defined(SVG_PARSER_LIBXML)
# include "parser_libxml.hpp"
#endif
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <map>

struct agg_rgba8_color_factory
{
  typedef agg::rgba8 color_type;

  static color_type create(unsigned char r, unsigned char g, unsigned char b)
  {
    return agg::rgba8(r, g, b);
  }
};

typedef svgpp::factory::color::percentage_adapter<agg_rgba8_color_factory> color_factory_t;
typedef svgpp::factory::length::unitless<> length_factory_t;
typedef boost::tuple<double, double, double, double> bounding_box_t;
typedef boost::function<bounding_box_t()> get_bounding_box_func_t;

