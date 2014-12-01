#pragma once

#define BOOST_PARAMETER_MAX_ARITY 15

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
#endif

#include <svgpp/factory/integer_color.hpp>
#include <svgpp/factory/unitless_length.hpp>
#if defined(SVG_PARSER_MSXML)
# include "parser_msxml.hpp"
#elif defined(SVG_PARSER_RAPIDXML_NS)
# include "parser_rapidxml_ns.hpp"
#elif defined(SVG_PARSER_LIBXML)
# include "parser_libxml.hpp"
#endif
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <map>

#if defined(RENDERER_AGG)
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
#elif defined(RENDERER_GDIPLUS)
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
#endif

typedef svgpp::factory::color::percentage_adapter<color_factory_base_t> color_factory_t;
typedef svgpp::factory::length::unitless<> length_factory_t;
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
  void path_move_to(double x, double y, svgpp::tag::coordinate::absolute const &)
  { 
    path_points_.push_back(Gdiplus::PointF(x, y));
    path_types_.push_back(Gdiplus::PathPointTypeStart);
  }

  void path_line_to(double x, double y, svgpp::tag::coordinate::absolute const &)
  { 
    path_points_.push_back(Gdiplus::PointF(x, y));
    path_types_.push_back(Gdiplus::PathPointTypeLine);
  }

  void path_cubic_bezier_to(
    double x1, double y1, 
    double x2, double y2, 
    double x, double y, 
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

  void path_quadratic_bezier_to(
    double x1, double y1, 
    double x, double y, 
    svgpp::tag::coordinate::absolute const &)
  { 
    path_points_.push_back(Gdiplus::PointF(x1, y1));
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