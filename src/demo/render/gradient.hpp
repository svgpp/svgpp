#pragma once

#include <boost/array.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <map>
#include <vector>
#include "common.hpp"
#if defined(RENDERER_SKIA)
#include <SkShader.h>
#endif

struct GradientStop
{
  GradientStop()
    : color_(BlackColor())
  {}

  number_t offset_;
  color_t color_;
};

typedef std::vector<GradientStop> GradientStops;

struct GradientBase
{
#if !defined(RENDERER_SKIA)
  enum SpreadMethod { spreadPad, spreadReflect, spreadRepeat };
#endif

  GradientBase()
#if defined(RENDERER_SKIA)
    : spreadMethod_(SkShader::kClamp_TileMode)
#else
    : spreadMethod_(spreadPad)
#endif
    , useObjectBoundingBox_(true)
  {}

  
#if defined(RENDERER_SKIA)
  boost::optional<SkMatrix> matrix_;
  SkShader::TileMode spreadMethod_;
#else
  boost::optional<boost::array<number_t, 6> > matrix_;
  SpreadMethod spreadMethod_;
#endif
  GradientStops stops_;
  bool useObjectBoundingBox_;
};

struct LinearGradient: GradientBase
{
  LinearGradient()
    : x1_(0)
    , y1_(0)
    , y2_(0)
  {}

  number_t x1_, y1_, x2_, y2_;
};

struct RadialGradient: GradientBase
{
  number_t cx_, cy_, r_, fx_, fy_;
};

typedef boost::variant<LinearGradient, RadialGradient> Gradient;

class Gradients
{
public:
  Gradients(XMLDocument & xml_document)
    : xml_document_(xml_document)
  {}

  boost::optional<Gradient> get(
    svg_string_t const & id, 
    length_factory_t const &/*, 
    get_bounding_box_func_t const & get_bounding_box*/);

private:
  XMLDocument & xml_document_;
};
