#pragma once

#define BOOST_PARAMETER_MAX_ARITY 17
// Following defines move parts of SVG++ code to svgpp_parser_impl.cpp file
// reducing compiler memory requirements
#define SVGPP_USE_EXTERNAL_PATH_DATA_PARSER
#define SVGPP_USE_EXTERNAL_TRANSFORM_PARSER
#define SVGPP_USE_EXTERNAL_PRESERVE_ASPECT_RATIO_PARSER
#define SVGPP_USE_EXTERNAL_PAINT_PARSER
#define SVGPP_USE_EXTERNAL_MISC_PARSER
#define SVGPP_USE_EXTERNAL_COLOR_PARSER
#define SVGPP_USE_EXTERNAL_LENGTH_PARSER

#if defined(RENDERER_AGG)
#include <agg_color_rgba.h>
#include <agg_trans_affine.h>
#elif defined(RENDERER_GDIPLUS)
#include <windows.h>
#include <gdiplus.h>
#undef min
#undef max
#undef small
#include <vector>
#elif defined(RENDERER_SKIA)
#include <SkPaint.h>
#endif

#include <svgpp/factory/integer_color.hpp>
#include <svgpp/factory/unitless_length.hpp>
#if defined(SVG_PARSER_MSXML)
# include "parser_msxml.hpp"
#elif defined(SVG_PARSER_RAPIDXML_NS)
# include "parser_rapidxml_ns.hpp"
#elif defined(SVG_PARSER_LIBXML)
# include "parser_libxml.hpp"
#elif defined(SVG_PARSER_XERCES)
# include "parser_xerces.hpp"
#else
#error One of XML parsers must be set with SVG_PARSER_xxxx macro
#endif
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <map>

#if defined(RENDERER_AGG)
typedef double number_t;
typedef agg::trans_affine transform_t;
typedef agg::rgba8 color_t;

inline color_t BlackColor() { return color_t(0, 0, 0); }
inline color_t TransparentBlackColor() { return color_t(0, 0, 0, 0); }
inline color_t TransparentWhiteColor() { return color_t(255, 255, 255, 0); }

struct color_factory_base_t
{
  typedef agg::rgba8 color_type;

  static color_type create(unsigned char r, unsigned char g, unsigned char b)
  {
    return color_type(r, g, b);
  }
};
typedef svgpp::factory::color::percentage_adapter<color_factory_base_t> color_factory_t;
#elif defined(RENDERER_GDIPLUS)
typedef Gdiplus::REAL number_t;
typedef Gdiplus::Matrix transform_t;
typedef Gdiplus::Color color_t;

inline color_t BlackColor() { return color_t(0, 0, 0); }
inline color_t TransparentBlackColor() { return color_t(0, 0, 0, 0); }
inline color_t TransparentWhiteColor() { return color_t(0, 255, 255, 255); }

struct color_factory_base_t
{
  typedef Gdiplus::Color color_type;

  static color_type create(unsigned char r, unsigned char g, unsigned char b)
  {
    return color_type(r, g, b);
  }
};
typedef svgpp::factory::color::percentage_adapter<color_factory_base_t> color_factory_t;
#elif defined(RENDERER_SKIA)
typedef SkScalar number_t;
typedef SkMatrix transform_t;
typedef SkColor color_t;

inline color_t BlackColor() { return SK_ColorBLACK; }
inline color_t TransparentBlackColor() { return SK_ColorTRANSPARENT; }
inline color_t TransparentWhiteColor() { return 0x00FFFFFF; }

struct color_policy
{
  static const SkColor preset_bits = 0xFF000000;
  static const SkColor r_offset = 16;
  static const SkColor g_offset = 8;
  static const SkColor b_offset = 0;
};

typedef svgpp::factory::color::integer<SkColor, color_policy> color_factory_t;
#else
#error One of renderers must be set with RENDERER_xxxx macro
#endif

typedef svgpp::factory::length::unitless<number_t, number_t> length_factory_t;
typedef boost::tuple<double, double, double, double> bounding_box_t;
typedef boost::function<bounding_box_t()> get_bounding_box_func_t;

#if defined(RENDERER_GDIPLUS)
inline void AssignMatrix(Gdiplus::Matrix & dest, Gdiplus::Matrix const & src)
{
  Gdiplus::REAL m[6];
  src.GetElements(m);
  dest.SetElements(m[0], m[1], m[2], m[3], m[4], m[5]);
}

class PathStorage
{
public:
  void path_move_to(number_t x, number_t y, svgpp::tag::coordinate::absolute const &)
  { 
    path_points_.push_back(Gdiplus::PointF(x, y));
    path_types_.push_back(Gdiplus::PathPointTypeStart);
  }

  void path_line_to(number_t x, number_t y, svgpp::tag::coordinate::absolute const &)
  { 
    path_points_.push_back(Gdiplus::PointF(x, y));
    path_types_.push_back(Gdiplus::PathPointTypeLine);
  }

  void path_cubic_bezier_to(
    number_t x1, number_t y1,
    number_t x2, number_t y2,
    number_t x, number_t y,
    svgpp::tag::coordinate::absolute const &)
  { 
    // TODO:
    path_points_.push_back(Gdiplus::PointF(x1, y1));
    path_types_.push_back(Gdiplus::PathPointTypeBezier);
    path_points_.push_back(Gdiplus::PointF(x2, y2));
    path_types_.push_back(Gdiplus::PathPointTypeBezier);
    path_points_.push_back(Gdiplus::PointF(x, y));
    path_types_.push_back(Gdiplus::PathPointTypeBezier);
  }

  void path_close_subpath()
  {
    if (!path_types_.empty())
      path_types_.back() |= Gdiplus::PathPointTypeCloseSubpath;
  }

  void path_exit()
  {}

  std::vector<Gdiplus::PointF> path_points_;
  std::vector<BYTE> path_types_;
};
#endif