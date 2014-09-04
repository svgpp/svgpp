#pragma once

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <agg_color_rgba.h>
#include "common.hpp"

typedef boost::variant<svgpp::tag::value::none, svgpp::tag::value::currentColor, agg::rgba8> SolidPaint;
struct IRIPaint
{
  IRIPaint(svg_string_t const & fragment, boost::optional<SolidPaint> const & fallback = boost::optional<SolidPaint>())
    : fragment_(fragment)
    , fallback_(fallback)
  {}

  svg_string_t fragment_;
  boost::optional<SolidPaint> fallback_;
};
typedef boost::variant<SolidPaint, IRIPaint> Paint;

struct InheritedStyle
{
  InheritedStyle()
    : color_(0, 0, 0)
    , stroke_width_(1.0)
    , stroke_opacity_(1.0)
    , fill_opacity_(1.0)
    , nonzero_fill_rule_(true)
    , line_cap_(agg::butt_cap)
    , line_join_(agg::miter_join)
    , miterlimit_(4.0)
    , fill_paint_(agg::rgba8(0, 0, 0))
    , stroke_paint_(svgpp::tag::value::none())
    , stroke_dashoffset_(0)
  {}

  agg::rgba8 color_;
  Paint fill_paint_, stroke_paint_;
  double stroke_width_;
  double stroke_opacity_, fill_opacity_;
  bool nonzero_fill_rule_;
  agg::line_cap_e line_cap_;
  agg::line_join_e line_join_;
  double miterlimit_;
  std::vector<double> stroke_dasharray_;
  double stroke_dashoffset_;
  boost::optional<svg_string_t> marker_start_, marker_mid_, marker_end_;
};

struct NoninheritedStyle
{
  NoninheritedStyle()
    : opacity_(1.0)
    , display_(true)
    , overflow_clip_(true) // TODO:
  {}

  double opacity_;
  bool display_;
  boost::optional<svg_string_t> mask_fragment_;
  boost::optional<svg_string_t> filter_;
  bool overflow_clip_;
};

struct Style: 
  InheritedStyle,
  NoninheritedStyle
{
  struct inherit_tag {};

  Style()
  {}

  Style(Style const & src, inherit_tag)
    : InheritedStyle(src)
  {}
};

template<class AttributeTag>
class PaintContext
{
public:
  PaintContext(Paint & paint)
    : paint_(paint)
  {}

  void set(AttributeTag, svgpp::tag::value::none)
  { paint_ = svgpp::tag::value::none(); }

  void set(AttributeTag, svgpp::tag::value::currentColor)
  { paint_ = svgpp::tag::value::currentColor(); }

  void set(AttributeTag, agg::rgba8 color, svgpp::tag::skip_icc_color = svgpp::tag::skip_icc_color())
  { paint_ = color; }

  template<class IRI>
  void set(AttributeTag tag, IRI const & iri)
  { 
    throw std::runtime_error("Non-local references aren't supported");
  }

  template<class IRI>
  void set(AttributeTag tag, svgpp::tag::iri_fragment, IRI const & fragment)
  { paint_ = IRIPaint(svg_string_t(boost::begin(fragment), boost::end(fragment))); }

  template<class IRI>
  void set(AttributeTag tag, IRI const &, svgpp::tag::value::none val)
  { set(tag, val); }

  template<class IRI>
  void set(AttributeTag tag, svgpp::tag::iri_fragment, IRI const & fragment, svgpp::tag::value::none val)
  { paint_ = IRIPaint(svg_string_t(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); }

  template<class IRI>
  void set(AttributeTag tag, IRI const &, svgpp::tag::value::currentColor val)
  { set(tag, val); }

  template<class IRI>
  void set(AttributeTag tag, svgpp::tag::iri_fragment, IRI const & fragment, svgpp::tag::value::currentColor val)
  { paint_ = IRIPaint(svg_string_t(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); }

  template<class IRI>
  void set(AttributeTag tag, IRI const &, agg::rgba8 val, svgpp::tag::skip_icc_color = svgpp::tag::skip_icc_color())
  { set(tag, val); }

  template<class IRI>
  void set(AttributeTag tag, svgpp::tag::iri_fragment, IRI const & fragment, agg::rgba8 val, svgpp::tag::skip_icc_color = svgpp::tag::skip_icc_color())
  { paint_ = IRIPaint(svg_string_t(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); }
  
protected:
  Paint & paint_;
};

class Stylable:
  public PaintContext<svgpp::tag::attribute::stroke>,
  public PaintContext<svgpp::tag::attribute::fill>
{
public:
  typedef PaintContext<svgpp::tag::attribute::stroke> stroke_paint;
  typedef PaintContext<svgpp::tag::attribute::fill> fill_paint;

  Stylable()
    : stroke_paint(style_.stroke_paint_)
    , fill_paint(style_.fill_paint_)
  {}

  Stylable(Stylable const & src)
    : stroke_paint(style_.stroke_paint_)
    , fill_paint(style_.fill_paint_)
    , style_(src.style_, Style::inherit_tag())
  {}

  using stroke_paint::set;
  using fill_paint::set;

  void set(svgpp::tag::attribute::display, svgpp::tag::value::none)
  { style().display_ = false; }

  void set(svgpp::tag::attribute::display, svgpp::tag::value::inherit)
  { style().display_ = parentStyle_.display_; }

  template<class ValueTag>
  void set(svgpp::tag::attribute::display, ValueTag)
  { style().display_ = true; }

  void set(svgpp::tag::attribute::color, agg::rgba8 val)
  { style().color_ = val; }

  void set(svgpp::tag::attribute::stroke_width, double val)
  { style().stroke_width_ = val; }

  void set(svgpp::tag::attribute::stroke_opacity, double val)
  { style().stroke_opacity_ = std::min(1.0, std::max(0.0, val)); }

  void set(svgpp::tag::attribute::fill_opacity, double val)
  { style().fill_opacity_ = std::min(1.0, std::max(0.0, val)); }

  void set(svgpp::tag::attribute::opacity, double val)
  { style().opacity_ = std::min(1.0, std::max(0.0, val)); }

  void set(svgpp::tag::attribute::opacity, svgpp::tag::value::inherit)
  { style().opacity_ = parentStyle_.opacity_; }

  void set(svgpp::tag::attribute::fill_rule, svgpp::tag::value::nonzero)
  { style().nonzero_fill_rule_ = true; }

  void set(svgpp::tag::attribute::fill_rule, svgpp::tag::value::evenodd)
  { style().nonzero_fill_rule_ = false; }

  void set(svgpp::tag::attribute::stroke_linecap, svgpp::tag::value::butt)
  { style().line_cap_ = agg::butt_cap; }

  void set(svgpp::tag::attribute::stroke_linecap, svgpp::tag::value::round)
  { style().line_cap_ = agg::round_cap; }

  void set(svgpp::tag::attribute::stroke_linecap, svgpp::tag::value::square)
  { style().line_cap_ = agg::square_cap; }

  void set(svgpp::tag::attribute::stroke_linejoin, svgpp::tag::value::miter)
  { style().line_join_ = agg::miter_join; }

  void set(svgpp::tag::attribute::stroke_linejoin, svgpp::tag::value::round)
  { style().line_join_ = agg::round_join; }

  void set(svgpp::tag::attribute::stroke_linejoin, svgpp::tag::value::bevel)
  { style().line_join_ = agg::bevel_join; }

  void set(svgpp::tag::attribute::stroke_miterlimit, double val)
  { style().miterlimit_ = val; }

  void set(svgpp::tag::attribute::stroke_dasharray, svgpp::tag::value::none)
  { style().stroke_dasharray_.clear(); }

  template<class Range>
  void set(svgpp::tag::attribute::stroke_dasharray, Range const & range)
  { 
    style().stroke_dasharray_.assign(boost::begin(range), boost::end(range)); 
  }

  void set(svgpp::tag::attribute::stroke_dashoffset, double val)
  { style().stroke_dashoffset_ = val; }

  template<class IRI>
  void set(svgpp::tag::attribute::mask, IRI const &)
  { throw std::runtime_error("Non-local references aren't supported"); }

  template<class IRI>
  void set(svgpp::tag::attribute::mask, svgpp::tag::iri_fragment, IRI const & fragment)
  { style().mask_fragment_ = svg_string_t(boost::begin(fragment), boost::end(fragment)); }

  void set(svgpp::tag::attribute::mask, svgpp::tag::value::none val)
  { style().mask_fragment_.reset(); }

  void set(svgpp::tag::attribute::mask, svgpp::tag::value::inherit val)
  { style().mask_fragment_ = parentStyle_.mask_fragment_; }

  Style & style() { return style_; }
  Style const & style() const { return style_; }

  template<class IRI>
  void set(svgpp::tag::attribute::marker_start, IRI const &)
  { 
    std::cout << "Non-local references aren't supported\n"; // Not error
    style().marker_start_.reset();
  }

  template<class IRI>
  void set(svgpp::tag::attribute::marker_start, svgpp::tag::iri_fragment, IRI const & fragment)
  { style().marker_start_ = svg_string_t(boost::begin(fragment), boost::end(fragment)); }

  void set(svgpp::tag::attribute::marker_start, svgpp::tag::value::none)
  { style().marker_start_.reset(); }

  template<class IRI>
  void set(svgpp::tag::attribute::marker_mid, IRI const &)
  { 
    std::cout << "Non-local references aren't supported\n"; // Not error
    style().marker_mid_.reset();
  }

  template<class IRI>
  void set(svgpp::tag::attribute::marker_mid, svgpp::tag::iri_fragment, IRI const & fragment)
  { style().marker_mid_ = svg_string_t(boost::begin(fragment), boost::end(fragment)); }

  void set(svgpp::tag::attribute::marker_mid, svgpp::tag::value::none)
  { style().marker_mid_.reset(); }

  template<class IRI>
  void set(svgpp::tag::attribute::marker_end, IRI const &)
  { 
    std::cout << "Non-local references aren't supported\n"; // Not error
    style().marker_end_.reset();
  }

  template<class IRI>
  void set(svgpp::tag::attribute::marker_end, svgpp::tag::iri_fragment, IRI const & fragment)
  { style().marker_end_ = svg_string_t(boost::begin(fragment), boost::end(fragment)); }

  void set(svgpp::tag::attribute::marker_end, svgpp::tag::value::none)
  { style().marker_end_.reset(); }

  template<class IRI>
  void set(svgpp::tag::attribute::marker, IRI const &)
  { 
    std::cout << "Non-local references aren't supported\n"; // Not error
    style().marker_start_.reset();
    style().marker_mid_.reset();
    style().marker_end_.reset();
  }

  template<class IRI>
  void set(svgpp::tag::attribute::marker, svgpp::tag::iri_fragment, IRI const & fragment)
  { 
    svg_string_t iri(boost::begin(fragment), boost::end(fragment));
    style().marker_start_ = iri; 
    style().marker_mid_   = iri; 
    style().marker_end_   = iri; 
  }

  void set(svgpp::tag::attribute::marker, svgpp::tag::value::none)
  { 
    style().marker_start_.reset();
    style().marker_mid_.reset();
    style().marker_end_.reset();
  }

  template<class IRI>
  void set(svgpp::tag::attribute::filter, IRI const &)
  { throw std::runtime_error("Non-local references aren't supported"); }

  template<class IRI>
  void set(svgpp::tag::attribute::filter, svgpp::tag::iri_fragment, IRI const & fragment)
  { style().filter_ = svg_string_t(boost::begin(fragment), boost::end(fragment)); }

  void set(svgpp::tag::attribute::filter, svgpp::tag::value::none val)
  { style().filter_.reset(); }

  void set(svgpp::tag::attribute::filter, svgpp::tag::value::inherit)
  { style().filter_ = parentStyle_.filter_; }

  void set(svgpp::tag::attribute::overflow, svgpp::tag::value::inherit)
  { style().overflow_clip_ = parentStyle_.overflow_clip_; }

  void set(svgpp::tag::attribute::overflow, svgpp::tag::value::visible)
  { style().overflow_clip_ = false; }

  void set(svgpp::tag::attribute::overflow, svgpp::tag::value::auto_)
  { style().overflow_clip_ = false; }

  void set(svgpp::tag::attribute::overflow, svgpp::tag::value::hidden)
  { style().overflow_clip_ = true; }

  void set(svgpp::tag::attribute::overflow, svgpp::tag::value::scroll)
  { style().overflow_clip_ = true; }

private:
  Style style_;
  NoninheritedStyle parentStyle_;
};
