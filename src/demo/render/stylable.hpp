#pragma once

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include "common.hpp"

typedef boost::variant<svgpp::tag::value::none, svgpp::tag::value::currentColor, color_t> SolidPaint;
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
    : color_(BlackColor())
    , fill_paint_(BlackColor())
    , stroke_paint_(svgpp::tag::value::none())
    , nonzero_fill_rule_(true)
    , stroke_opacity_(1.0)
    , fill_opacity_(1.0)
#if !defined(RENDERER_SKIA)
    , stroke_width_(1.0)
#if defined(RENDERER_AGG)
    , line_cap_(agg::butt_cap)
    , line_join_(agg::miter_join)
#elif defined(RENDERER_GDIPLUS)
    , line_cap_(Gdiplus::LineCapSquare)
    , line_join_(Gdiplus::LineJoinMiterClipped)
#endif
    , miterlimit_(4.0)
    , stroke_dashoffset_(0)
#endif
  {
#if defined(RENDERER_SKIA)
    skPaintStroke_.setAntiAlias(true);
    skPaintStroke_.setStyle(SkPaint::kStroke_Style);
    skPaintStroke_.setStrokeWidth(1);
    skPaintStroke_.setStrokeMiter(4);
    skPaintStroke_.setStrokeCap(SkPaint::kButt_Cap);
    skPaintStroke_.setStrokeJoin(SkPaint::kMiter_Join);
#endif
  }

  color_t color_;
  Paint fill_paint_, stroke_paint_;
  number_t stroke_opacity_, fill_opacity_;
  bool nonzero_fill_rule_;
#if defined(RENDERER_SKIA)
  SkPaint skPaintStroke_;
#else
  number_t stroke_width_;
#if defined(RENDERER_AGG)
  agg::line_cap_e line_cap_;
  agg::line_join_e line_join_;
#elif defined(RENDERER_GDIPLUS)
  Gdiplus::LineCap line_cap_;
  Gdiplus::LineJoin line_join_;
#endif 
  number_t miterlimit_;
#endif
  std::vector<number_t> stroke_dasharray_;
  number_t stroke_dashoffset_;
  boost::optional<svg_string_t> marker_start_, marker_mid_, marker_end_;
};

struct NoninheritedStyle
{
  NoninheritedStyle()
    : opacity_(1.0)
    , display_(true)
    , overflow_clip_(true) // TODO:
  {}

  number_t opacity_;
  bool display_;
  boost::optional<svg_string_t> mask_fragment_, clip_path_fragment_;
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

  void set(AttributeTag, color_t color, svgpp::tag::skip_icc_color = svgpp::tag::skip_icc_color())
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
  void set(AttributeTag tag, IRI const &, color_t val, svgpp::tag::skip_icc_color = svgpp::tag::skip_icc_color())
  { set(tag, val); }

  template<class IRI>
  void set(AttributeTag tag, svgpp::tag::iri_fragment, IRI const & fragment, color_t val, svgpp::tag::skip_icc_color = svgpp::tag::skip_icc_color())
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

  void set(svgpp::tag::attribute::color, color_t val)
  { style().color_ = val; }

  void set(svgpp::tag::attribute::stroke_width, number_t val)
#if defined(RENDERER_SKIA)
  { style().skPaintStroke_.setStrokeWidth(val); }
#else
  { style().stroke_width_ = val; }
#endif

  void set(svgpp::tag::attribute::stroke_opacity, number_t val)
  { style().stroke_opacity_ = std::min(number_t(1), std::max(number_t(0), val)); }

  void set(svgpp::tag::attribute::fill_opacity, number_t val)
  { style().fill_opacity_ = std::min(number_t(1), std::max(number_t(0), val)); }

  void set(svgpp::tag::attribute::opacity, number_t val)
  { style().opacity_ = std::min(number_t(1), std::max(number_t(0), val)); }

  void set(svgpp::tag::attribute::opacity, svgpp::tag::value::inherit)
  { style().opacity_ = parentStyle_.opacity_; }

  void set(svgpp::tag::attribute::fill_rule, svgpp::tag::value::nonzero)
  { style().nonzero_fill_rule_ = true; }

  void set(svgpp::tag::attribute::fill_rule, svgpp::tag::value::evenodd)
  { style().nonzero_fill_rule_ = false; }

  void set(svgpp::tag::attribute::stroke_linecap, svgpp::tag::value::butt)
#if defined(RENDERER_AGG)
  { style().line_cap_ = agg::butt_cap; }
#elif defined(RENDERER_GDIPLUS)
  { style().line_cap_ = Gdiplus::LineCapFlat; }
#elif defined(RENDERER_SKIA)
  { style().skPaintStroke_.setStrokeCap(SkPaint::kButt_Cap); }
#endif

  void set(svgpp::tag::attribute::stroke_linecap, svgpp::tag::value::round)
#if defined(RENDERER_AGG)
  { style().line_cap_ = agg::round_cap; }
#elif defined(RENDERER_GDIPLUS)
  { style().line_cap_ = Gdiplus::LineCapRound; }
#elif defined(RENDERER_SKIA)
  { style().skPaintStroke_.setStrokeCap(SkPaint::kRound_Cap); }
#endif

  void set(svgpp::tag::attribute::stroke_linecap, svgpp::tag::value::square)
#if defined(RENDERER_AGG)
  { style().line_cap_ = agg::square_cap; }
#elif defined(RENDERER_GDIPLUS)
  { style().line_cap_ = Gdiplus::LineCapSquare; }
#elif defined(RENDERER_SKIA)
  { style().skPaintStroke_.setStrokeCap(SkPaint::kSquare_Cap); }
#endif

  void set(svgpp::tag::attribute::stroke_linejoin, svgpp::tag::value::miter)
#if defined(RENDERER_AGG)
  { style().line_join_ = agg::miter_join; }
#elif defined(RENDERER_GDIPLUS)
  { style().line_join_ = Gdiplus::LineJoinMiterClipped; }
#elif defined(RENDERER_SKIA)
  { style().skPaintStroke_.setStrokeJoin(SkPaint::kMiter_Join); }
#endif

  void set(svgpp::tag::attribute::stroke_linejoin, svgpp::tag::value::round)
#if defined(RENDERER_AGG)
  { style().line_join_ = agg::round_join; }
#elif defined(RENDERER_GDIPLUS)
  { style().line_join_ = Gdiplus::LineJoinRound; }
#elif defined(RENDERER_SKIA)
  { style().skPaintStroke_.setStrokeJoin(SkPaint::kRound_Join); }
#endif

  void set(svgpp::tag::attribute::stroke_linejoin, svgpp::tag::value::bevel)
#if defined(RENDERER_AGG)
  { style().line_join_ = agg::bevel_join; }
#elif defined(RENDERER_GDIPLUS)
  { style().line_join_ = Gdiplus::LineJoinBevel; }
#elif defined(RENDERER_SKIA)
  { style().skPaintStroke_.setStrokeJoin(SkPaint::kBevel_Join); }
#endif

  void set(svgpp::tag::attribute::stroke_miterlimit, number_t val)
#if defined(RENDERER_SKIA)
  { style().skPaintStroke_.setStrokeMiter(val); }
#else
  { style().miterlimit_ = val; }
#endif

  void set(svgpp::tag::attribute::stroke_dasharray, svgpp::tag::value::none)
  { style().stroke_dasharray_.clear(); }

  template<class Range>
  void set(svgpp::tag::attribute::stroke_dasharray, Range const & range)
  { 
    style().stroke_dasharray_.assign(boost::begin(range), boost::end(range)); 
  }

  void set(svgpp::tag::attribute::stroke_dashoffset, number_t val)
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

  template<class IRI>
  void set(svgpp::tag::attribute::clip_path, IRI const &)
  { throw std::runtime_error("Non-local references aren't supported"); }

  template<class IRI>
  void set(svgpp::tag::attribute::clip_path, svgpp::tag::iri_fragment, IRI const & fragment)
  { style().clip_path_fragment_ = svg_string_t(boost::begin(fragment), boost::end(fragment)); }

  void set(svgpp::tag::attribute::clip_path, svgpp::tag::value::none val)
  { style().clip_path_fragment_.reset(); }

  void set(svgpp::tag::attribute::clip_path, svgpp::tag::value::inherit val)
  { style().clip_path_fragment_ = parentStyle_.clip_path_fragment_; }

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
