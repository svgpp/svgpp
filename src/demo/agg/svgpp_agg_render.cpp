#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_SET_SIZE 50

#include "common.hpp"

#include <svgpp/document_traversal.hpp>
#include <svgpp/utility/gil_utility.h>

#include <boost/bind.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/optional.hpp>
#include <boost/scope_exit.hpp>

#include <agg_bounding_rect.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_rendering_buffer.h>
#include <agg_renderer_base.h>
#include <agg_renderer_primitives.h>
#include <agg_renderer_scanline.h>
#include <agg_pixfmt_rgba.h>
#include <agg_scanline_p.h>
#include <agg_conv_stroke.h>
#include <agg_conv_contour.h>
#include <agg_conv_curve.h>
#include <agg_conv_dash.h>
#include <agg_path_storage.h>
#include <agg_pixfmt_amask_adaptor.h>
#include <agg_span_allocator.h>
#include <agg_span_gradient.h>
#include <agg_span_interpolator_linear.h>

#include <map>
#include <set>
#include <fstream>
#include <numeric>

#include "bmp_header.hpp"
#include "stylable.hpp"
#include "gradient.hpp"
#include "clip_path.hpp"
#include "filter.hpp"

class Transformable;
class Canvas;
class Path;
class Use;
class Switch;
class ReferencedSymbolOrSvg;
class Mask;
class Marker;

typedef agg::pixfmt_rgba32 pixfmt_t;
typedef agg::pixfmt_amask_adaptor<pixfmt_t, ClipBuffer::alpha_mask_t> pixfmt_amask_adaptor_t;
typedef agg::renderer_base<pixfmt_amask_adaptor_t> renderer_base_amask_t;

struct Document
{
  class FollowRef;

  Document(XMLDocument & xmlDocument)
    : xml_document_(xmlDocument)
    , gradients_(xml_document_)
    , filters_(xml_document_)
  {}

  XMLDocument & xml_document_;
  Gradients gradients_;
  Filters filters_;
  typedef std::set<XMLElement> followed_refs_t;
  followed_refs_t followed_refs_;
};

class Document::FollowRef
{
public:
  FollowRef(Document & document, XMLElement const & el)
    : document_(document)
  {
    std::pair<Document::followed_refs_t::iterator, bool> ins = document.followed_refs_.insert(el);
    if (!ins.second)
      throw std::runtime_error("Cyclic reference found");
    lock_ = ins.first;
  }

  ~FollowRef()
  {
    document_.followed_refs_.erase(lock_);
  }

private:
  Document & document_;
  Document::followed_refs_t::iterator lock_;
};

struct path_policy: svgpp::policy::path::no_shorthands
{
  static const bool arc_as_cubic_bezier = true; 
};

struct child_context_factories
{
  template<class ParentContext, class ElementTag, class Enable = void>
  struct apply;
};

template<>
struct child_context_factories::apply<Canvas, svgpp::tag::element::svg, void>
{
  typedef svgpp::factory::context::on_stack<Canvas, Canvas> type;
};

template<>
struct child_context_factories::apply<Canvas, svgpp::tag::element::g, void>
{
  typedef svgpp::factory::context::on_stack<Canvas, Canvas> type;
};

template<>
struct child_context_factories::apply<Canvas, svgpp::tag::element::a, void>
{
  typedef svgpp::factory::context::on_stack<Canvas, Canvas> type;
};

template<>
struct child_context_factories::apply<Canvas, svgpp::tag::element::switch_, void>
{
  typedef svgpp::factory::context::on_stack<Canvas, Switch> type;
};

template<class ElementTag>
struct child_context_factories::apply<Switch, ElementTag, void>: apply<Canvas, ElementTag>
{};

template<>
struct child_context_factories::apply<Canvas, svgpp::tag::element::use_, void>
{
  typedef svgpp::factory::context::on_stack<Canvas, Use> type;
};

template<class ElementTag>
struct child_context_factories::apply<Canvas, ElementTag, typename boost::enable_if<boost::mpl::has_key<svgpp::traits::shape_elements, ElementTag> >::type>
{
  typedef svgpp::factory::context::on_stack<Canvas, Path> type;
};

// For referenced by 'use' elements
template<>
struct child_context_factories::apply<Use, svgpp::tag::element::svg, void>
{
  typedef svgpp::factory::context::on_stack<Use, ReferencedSymbolOrSvg> type;
};

template<>
struct child_context_factories::apply<Use, svgpp::tag::element::symbol, void>
{
  typedef svgpp::factory::context::on_stack<Use, ReferencedSymbolOrSvg> type;
};

template<class ElementTag>
struct child_context_factories::apply<Use, ElementTag, void>: child_context_factories::apply<Canvas, ElementTag>
{};

template<class ElementTag>
struct child_context_factories::apply<ReferencedSymbolOrSvg, ElementTag, void>: child_context_factories::apply<Canvas, ElementTag>
{};

// 'mask'
template<>
struct child_context_factories::apply<Mask, svgpp::tag::element::mask, void>
{
  typedef svgpp::factory::context::same<Mask, svgpp::tag::element::mask> type;
};

template<class ElementTag>
struct child_context_factories::apply<Mask, ElementTag, void>: apply<Canvas, ElementTag>
{};

// 'marker'
template<>
struct child_context_factories::apply<Marker, svgpp::tag::element::marker, void>
{
  typedef svgpp::factory::context::same<Marker, svgpp::tag::element::marker> type;
};

template<class ElementTag>
struct child_context_factories::apply<Marker, ElementTag, void>: child_context_factories::apply<Canvas, ElementTag>
{};

struct DocumentTraversalControl
{
  static bool proceed_to_element_content(Stylable const & context)
  {
    return context.style().display_;
  }

  template<class Context>
  static bool proceed_to_next_child(Context &)
  {
    return true;
  }
};

typedef boost::mpl::set<
  svgpp::tag::element::svg,
  svgpp::tag::element::g,
  svgpp::tag::element::switch_,
  svgpp::tag::element::a,
  svgpp::tag::element::use_,
  svgpp::tag::element::path,
  svgpp::tag::element::rect,
  svgpp::tag::element::line,
  svgpp::tag::element::circle,
  svgpp::tag::element::ellipse,
  svgpp::tag::element::polyline,
  svgpp::tag::element::polygon
>::type processed_elements;

typedef boost::mpl::fold<
  boost::mpl::protect<
    boost::mpl::joint_view<
      svgpp::traits::shapes_attributes_by_element, 
      svgpp::traits::viewport_attributes
    >
  >,
  boost::mpl::set<
    svgpp::tag::attribute::display,
    svgpp::tag::attribute::transform,
    svgpp::tag::attribute::color,
    svgpp::tag::attribute::fill,
    svgpp::tag::attribute::fill_opacity,
    svgpp::tag::attribute::fill_rule,
    svgpp::tag::attribute::filter,
    svgpp::tag::attribute::marker_start,
    svgpp::tag::attribute::marker_mid,
    svgpp::tag::attribute::marker_end,
    svgpp::tag::attribute::markerUnits,
    svgpp::tag::attribute::markerWidth,
    svgpp::tag::attribute::markerHeight,
    svgpp::tag::attribute::mask,
    svgpp::tag::attribute::maskUnits,
    svgpp::tag::attribute::maskContentUnits,
    svgpp::tag::attribute::refX,
    svgpp::tag::attribute::refY,
    svgpp::tag::attribute::stroke,
    svgpp::tag::attribute::stroke_width,
    svgpp::tag::attribute::stroke_opacity,
    svgpp::tag::attribute::stroke_linecap,
    svgpp::tag::attribute::stroke_linejoin,
    svgpp::tag::attribute::stroke_miterlimit,
    svgpp::tag::attribute::stroke_dasharray,
    svgpp::tag::attribute::stroke_dashoffset,
    svgpp::tag::attribute::opacity,
    svgpp::tag::attribute::orient,
    boost::mpl::pair<svgpp::tag::element::use_, svgpp::tag::attribute::xlink::href>,
    boost::mpl::pair<svgpp::tag::element::use_, svgpp::tag::attribute::x>,
    boost::mpl::pair<svgpp::tag::element::use_, svgpp::tag::attribute::y>
  >::type,
  boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
>::type processed_attributes;

class ImageBuffer: boost::noncopyable
{
public:
  ImageBuffer()
  {}

  ImageBuffer(int width, int height)
    : buffer_(width * height * pixfmt_t::pix_width)
    , rbuf_(&buffer_[0], width, height, width * pixfmt_t::pix_width)
    , pixfmt_(rbuf_)
  {
    agg::renderer_base<pixfmt_t> renderer_base(pixfmt_);
    renderer_base.clear(pixfmt_t::color_type(0, 0, 0, 0));
  }

  pixfmt_t & pixfmt() { return pixfmt_; }

  bool isSizeSet() const { return !buffer_.empty(); }

  void setSize(int width, int height, pixfmt_t::color_type const & fill_color)
  {
    BOOST_ASSERT(buffer_.empty());
    buffer_.resize(width * height * pixfmt_t::pix_width);
    rbuf_.attach(&buffer_[0], width, height, width * pixfmt_t::pix_width);
    pixfmt_.attach(rbuf_);
    agg::renderer_base<pixfmt_t> renderer_base(pixfmt_);
    renderer_base.clear(fill_color);
  }

  boost::gil::rgba8_view_t gilView()
  {
    return boost::gil::interleaved_view(rbuf_.width(), rbuf_.height(), 
      reinterpret_cast<boost::gil::rgba8_pixel_t*>(&buffer_[0]), rbuf_.stride());
  }

private:
  std::vector<unsigned char> buffer_;
  agg::rendering_buffer rbuf_;
  pixfmt_t pixfmt_;
};

inline boost::gil::rgba8_view_t gilView(pixfmt_t & pixfmt)
{
  return boost::gil::interleaved_view(pixfmt.width(), pixfmt.height(), 
    reinterpret_cast<boost::gil::rgba8_pixel_t*>(pixfmt.row_ptr(0)), pixfmt.stride());
}

class Transformable
{
public:
  void set_transform_matrix(const boost::array<double, 6> & matrix)
  {
    transform_.premultiply(agg::trans_affine(matrix.data()));
  }

  agg::trans_affine       & transform()       { return transform_; }
  agg::trans_affine const & transform() const { return transform_; }

private:
  agg::trans_affine transform_;
};

typedef boost::function<pixfmt_t&()> lazy_pixfmt_t;

class Canvas: 
  public Stylable,
  public Transformable
{
public:
  struct dontInheritStyle {};

  Canvas(Document & document, ImageBuffer & image_buffer)
    : document_(document)
    , parent_pixfmt_(boost::bind(&ImageBuffer::pixfmt, boost::ref(image_buffer)))
    , image_buffer_(&image_buffer)
    , is_switch_child_(false)
  {
    if (image_buffer.isSizeSet())
      clip_buffer_.reset(new ClipBuffer(image_buffer_->pixfmt().width(), image_buffer_->pixfmt().height()));
  }

  Canvas(Canvas & parent)
    : Transformable(parent)
    , Stylable(parent)
    , document_(parent.document_)
    , image_buffer_(NULL)
    , parent_pixfmt_(boost::bind(&Canvas::getPixfmt, &parent))
    , is_switch_child_(parent.isSwitchElement())
    , length_factory_(parent.length_factory_)
    , clip_buffer_(parent.clip_buffer_)
  {}

  Canvas(Canvas & parent, dontInheritStyle)
    : Transformable(parent)
    , document_(parent.document_)
    , image_buffer_(NULL)
    , parent_pixfmt_(boost::bind(&Canvas::getPixfmt, &parent))
    , is_switch_child_(false)
    , length_factory_(parent.length_factory_)
    , clip_buffer_(parent.clip_buffer_)
  {}

  void on_exit_element()
  {
    if (!own_buffer_.get())
      return;
    applyFilter();
    if (style().mask_fragment_)
    {
      pixfmt_t & parent_pixfmt = parent_pixfmt_();
      ImageBuffer mask_buffer(parent_pixfmt.width(), parent_pixfmt.height());
      loadMask(mask_buffer);
      typedef boost::gil::color_converted_view_type<
        boost::gil::rgba8_view_t, 
        boost::gil::gray8_pixel_t, 
        svgpp::gil_utility::rgba_to_mask_color_converter<> 
      >::type mask_view_t;
      mask_view_t mask_view = boost::gil::color_converted_view<boost::gil::gray8_pixel_t>(
        mask_buffer.gilView(), svgpp::gil_utility::rgba_to_mask_color_converter<>());

      boost::gil::rgba8_view_t own_view = own_buffer_->gilView();
      boost::gil::rgba8_view_t::iterator o = own_view.begin();
      for(mask_view_t::iterator m = mask_view.begin(); m !=mask_view.end(); ++m, ++o)
      {
        using namespace boost::gil;
        get_color(*o, alpha_t()) = 
          channel_multiply(
            get_color(*o, alpha_t()),
            get_color(*m, gray_color_t())
          );
      }
      BOOST_ASSERT(o == own_view.end());
    }
    agg::renderer_base<pixfmt_t> renderer_base(parent_pixfmt_());
    renderer_base.blend_from(own_buffer_->pixfmt(), NULL, 0, 0, unsigned(style().opacity_ * 255));
  }

  void set_viewport(double viewport_x, double viewport_y, double viewport_width, double viewport_height)
  {
    if (image_buffer_) // If topmost SVG element
    {
      image_buffer_->setSize(viewport_width + 1.0, viewport_height + 1.0, pixfmt_t::color_type(255, 255, 255, 0));
      clip_buffer_.reset(new ClipBuffer(image_buffer_->pixfmt().width(), image_buffer_->pixfmt().height()));
    }
    else
    {
      if (!clip_buffer_.unique())
        clip_buffer_.reset(new ClipBuffer(*clip_buffer_));
      clip_buffer_->intersectClipRect(transform(), viewport_x, viewport_y, viewport_width, viewport_height);
    }
    length_factory_.set_viewport_size(viewport_width, viewport_height);
  }

  length_factory_t & length_factory() 
  { return length_factory_; }

  length_factory_t const & length_factory() const
  { return length_factory_; }

private:
  Document & document_;
  ImageBuffer * const image_buffer_; // Non-NULL only for topmost SVG element
  lazy_pixfmt_t parent_pixfmt_;
  std::auto_ptr<ImageBuffer> own_buffer_;
  boost::shared_ptr<ClipBuffer> clip_buffer_;
  length_factory_t length_factory_;

  void loadMask(ImageBuffer &) const;
  void applyFilter();

protected:
  pixfmt_t & getPixfmt()
  {
    pixfmt_t & parent_pixfmt = parent_pixfmt_();

    if (style().opacity_ < 0.999 || style().mask_fragment_ || style().filter_)
    {
      if (!own_buffer_.get())
        own_buffer_.reset(new ImageBuffer(parent_pixfmt.width(), parent_pixfmt.height()));
      return own_buffer_->pixfmt();
    }
    return parent_pixfmt;
  }

  Document & document() const { return document_; }
  ClipBuffer const & clipBuffer() const { return *clip_buffer_; }
  virtual bool isSwitchElement() const { return false; }

  const bool is_switch_child_;
};

void Canvas::applyFilter()
{
  if (!style().filter_)
    return;

  struct View: IFilterView
  {
    View(boost::gil::rgba8c_view_t const & v)
      : view_(v)
    {}

    virtual boost::gil::rgba8c_view_t view() { return view_; }

  private:
    boost::gil::rgba8c_view_t view_;
  };

  Filters::Input in;
  in.sourceGraphic_ = IFilterViewPtr(new View(own_buffer_->gilView()));
  in.backgroundImage_ = IFilterViewPtr(new View(gilView(parent_pixfmt_())));
  IFilterViewPtr out = document_.filters_.get(*style().filter_, length_factory_, in);
  if (out)
    boost::gil::copy_pixels(out->view(), own_buffer_->gilView());
}

class Switch: public Canvas
{
public:
  Switch(Canvas & parent)
    : Canvas(parent)
  {}

  virtual bool isSwitchElement() const { return true; }
};

class Path: public Canvas
{
public:
  Path(Canvas & parent)
    : Canvas(parent)
  {}

  void on_exit_element()
  {
    if (style().display_)
    {
      if (path_storage_.total_vertices() > 0)
        drawPath();
      drawMarkers();
    }
    Canvas::on_exit_element();
  }

  void path_move_to(double x, double y, svgpp::tag::absolute_coordinate const &)
  { 
    path_storage_.move_to(x, y);
  }

  void path_line_to(double x, double y, svgpp::tag::absolute_coordinate const &)
  { 
    path_storage_.line_to(x, y);
  }

  void path_cubic_bezier_to(
    double x1, double y1, 
    double x2, double y2, 
    double x, double y, 
    svgpp::tag::absolute_coordinate const &)
  { 
    path_storage_.curve4(x1, y1, x2, y2, x, y);
  }

  void path_quadratic_bezier_to(
    double x1, double y1, 
    double x, double y, 
    svgpp::tag::absolute_coordinate const &)
  { 
    path_storage_.curve3(x1, y1, x, y);
  }

  void path_close_subpath()
  {
    path_storage_.end_poly(agg::path_flags_close);
  }

  void path_exit()
  {
  }

  void marker(svgpp::marker_vertex v, double x, double y, double directionality, unsigned marker_index)
  {
    if (marker_index >= markers_.size())
      markers_.resize(marker_index + 1);
    MarkerPos & m = markers_[marker_index];
    m.v = v;
    m.x = x;
    m.y = y;
    m.directionality = directionality;
  }

  void marker(svgpp::marker_vertex v, double x, double y, svgpp::tag::orient_fixed, unsigned marker_index)
  {
    BOOST_ASSERT(false); // Auto directionality requested in marker_get_config
  }

  void marker_get_config(svgpp::marker_config & start, svgpp::marker_config & mid, svgpp::marker_config & end)
  {
    start = svgpp::marker_orient_auto;
    mid   = svgpp::marker_orient_auto;
    end   = svgpp::marker_orient_auto;
  }

private:
  agg::path_storage path_storage_;

  struct MarkerPos
  {
    svgpp::marker_vertex v;
    double x, y, directionality;
  };

  typedef std::vector<MarkerPos> Markers; 
  Markers markers_;

  boost::optional<svg_string_t> & getMarkerReference(svgpp::marker_vertex v)
  {
    switch (v)
    {
    default:
      BOOST_ASSERT(false);
    case svgpp::marker_start:
      return style().marker_start_;
    case svgpp::marker_mid:
      return style().marker_mid_;
    case svgpp::marker_end:
      return style().marker_end_;
    }
  }

  typedef boost::variant<svgpp::tag::value::none, agg::rgba8, Gradient> EffectivePaint;
  template<class VertexSource>
  void paintScanlines(EffectivePaint const & paint, double opacity, agg::rasterizer_scanline_aa<> & rasterizer,
    VertexSource & curved);
  template<class VertexSourceStroked, class VertexSourceCurved>
  void strokePath(EffectivePaint const & stroke, VertexSourceStroked & curved_stroked, VertexSourceCurved & curved);
  void drawPath();
  void drawMarkers();
  void drawMarker(svg_string_t const & id, double x, double y, double dir);
  EffectivePaint getEffectivePaint(Paint const &) const;
};

struct afterMarkerUnitsTag {};

struct attribute_traversal: svgpp::policy::attribute_traversal::default_policy
{
  typedef boost::mpl::if_<
    boost::is_same<boost::mpl::_1, svgpp::tag::element::marker>,
    boost::mpl::vector<
      svgpp::tag::attribute::markerUnits,
      svgpp::tag::attribute::orient,
      svgpp::notify_context<afterMarkerUnitsTag>
    >,
    boost::mpl::empty_sequence
  > get_priority_attributes_by_element;
};

typedef 
  svgpp::document_traversal<
    svgpp::context_factories<child_context_factories>,
    svgpp::length_policy<svgpp::policy::length::forward_to_method<Canvas, const length_factory_t> >,
    svgpp::color_factory<color_factory_t>,
    svgpp::processed_elements<processed_elements>,
    svgpp::processed_attributes<processed_attributes>,
    svgpp::path_policy<path_policy>,
    svgpp::document_traversal_control_policy<DocumentTraversalControl>,
    svgpp::load_transform_policy<svgpp::policy::load_transform::forward_to_method<Transformable> >, // Same as default, but less instantiations
    svgpp::load_path_policy<svgpp::policy::load_path::forward_to_method<Path> >, // Same as default, but less instantiations
    svgpp::error_policy<svgpp::policy::error::default_policy<Stylable> >, // Type of context isn't used
    svgpp::markers_policy<svgpp::policy::markers::calculate>,
    svgpp::attribute_traversal_policy<attribute_traversal>
  > document_traversal_main;

class Use: public Canvas
{
public:
  Use(Canvas & parent)
    : Canvas(parent)
    , x_(0)
    , y_(0)
  {}

  void on_exit_element()
  {
    if (!style().display_)
      return;
    if (XMLElement element = document().xml_document_.findElementById(fragment_id_))
    {
      Document::FollowRef lock(document(), element);
      transform().premultiply(agg::trans_affine_translation(x_, y_));
      document_traversal_main::load_referenced_element<
        svgpp::referencing_element<svgpp::tag::element::use_>,
        svgpp::expected_elements<svgpp::traits::reusable_elements>,
        svgpp::processed_elements<
          boost::mpl::insert<processed_elements, svgpp::tag::element::symbol>::type 
        >
      >::load(element, *this);
    }
    else
      std::cerr << "Element referenced by 'use' not found\n";
    Canvas::on_exit_element();
  }

  using Canvas::set;

  template<class IRI>
  void set(svgpp::tag::attribute::xlink::href, svgpp::tag::iri_fragment, IRI const & fragment)
  { fragment_id_.assign(boost::begin(fragment), boost::end(fragment)); }

  template<class IRI>
  void set(svgpp::tag::attribute::xlink::href, IRI const & fragment)
  { std::cerr << "External references aren't supported\n"; }

  void set(svgpp::tag::attribute::x, double val)
  { x_ = val; }

  void set(svgpp::tag::attribute::y, double val)
  { y_ = val; }

  void set(svgpp::tag::attribute::width, double val)
  { width_ = val; }

  void set(svgpp::tag::attribute::height, double val)
  { height_ = val; }

  boost::optional<double> const & width() const { return width_; }
  boost::optional<double> const & height() const { return height_; }

private:
  svg_string_t fragment_id_;
  double x_, y_;
  boost::optional<double> width_, height_;
};

class ReferencedSymbolOrSvg: 
  public Canvas
{
public:
  ReferencedSymbolOrSvg(Use & parent)
    : Canvas(parent)
    , parent_(parent)
  {
  }

  void get_reference_viewport_size(double & width, double & height)
  {
    if (parent_.width())
      width = *parent_.width();
    if (parent_.height())
      height = *parent_.height();
  }

private:
  Use & parent_;
};

class Mask: public Canvas
{
public:
  Mask(Document & document, ImageBuffer & image_buffer, Transformable const & referenced)
    : Canvas(document, image_buffer)
    , maskUseObjectBoundingBox_(true)
    , maskContentUseObjectBoundingBox_(false)
  {
    transform() = referenced.transform();
  }

  void on_enter_element(svgpp::tag::element::mask) 
  {}

  void on_exit_element()
  {}

  using Canvas::set;

  void set(svgpp::tag::attribute::maskUnits, svgpp::tag::value::userSpaceOnUse)
  { maskUseObjectBoundingBox_ = false; }

  void set(svgpp::tag::attribute::maskUnits, svgpp::tag::value::objectBoundingBox)
  { maskUseObjectBoundingBox_ = true; }

  void set(svgpp::tag::attribute::maskContentUnits, svgpp::tag::value::userSpaceOnUse)
  { maskContentUseObjectBoundingBox_ = false; }

  void set(svgpp::tag::attribute::maskContentUnits, svgpp::tag::value::objectBoundingBox)
  { maskContentUseObjectBoundingBox_ = true; }

  void set(svgpp::tag::attribute::x, double val)
  { x_ = val; }

  void set(svgpp::tag::attribute::y, double val)
  { y_ = val; }

  void set(svgpp::tag::attribute::width, double val)
  { width_ = val; }

  void set(svgpp::tag::attribute::height, double val)
  { height_ = val; }

private:
  bool maskUseObjectBoundingBox_, maskContentUseObjectBoundingBox_;
  double x_, y_, width_, height_; // TODO: defaults
};

void Canvas::loadMask(ImageBuffer & mask_buffer) const
{
  if (XMLElement element = document().xml_document_.findElementById(*style().mask_fragment_))
  {
    Document::FollowRef lock(document(), element);

    Mask mask(document_, mask_buffer, *this);
    document_traversal_main::load_referenced_element<
      svgpp::expected_elements<boost::mpl::set1<svgpp::tag::element::mask> > 
    >::load(element, mask);
  }
  else
    throw std::runtime_error("Element referenced by 'mask' not found");
}

template<class Gradient>
class GradientRepeatAdapter
{
public:
  GradientRepeatAdapter(Gradient const & gradient, GradientBase::SpreadMethod method)
    : gradient_(gradient)
    , method_(method)
  {}

  int calculate(int x, int y, int d) const
  {
    int val = gradient_.calculate(x, y, d);
    switch(method_)
    {
    default:
      assert(false);
    case GradientBase::spreadPad:
    {
      if (val < 0)
        return 0;
      if (val > d)
        return d;
      return x;
    }
    case GradientBase::spreadReflect:
    {
      int d2 = d * 2;
      int ret = val % d2;
      if (ret < 0) ret += d2;
      if (ret >= d) ret = d2 - ret;
      return ret;
    }
    case GradientBase::spreadRepeat:
    {
      int ret = val % d;
      if (ret < 0) ret += d;
      return ret;
    }
    }
  }

private:
  Gradient const & gradient_;
  GradientBase::SpreadMethod const method_;
};

struct ColorFunctionProfile
{
  static const unsigned size_ = 256;

  ColorFunctionProfile(GradientStops const & stops, double opacity) 
  {
    assert(stops.size() >= 2);

    static const double offset_step = 1.0 / size_;
    double offset = 0;
    GradientStops::const_iterator stop1 = stops.begin(), stop2 = stops.begin();
    agg::rgba8 color1 = stopColor(*stop1, opacity), color2 = color1;
    for(int i = 0; i < size_; ++i, offset += offset_step)
    {
      while(stop2 != stops.end() && offset > stop2->offset_)
      {
        stop1 = stop2;
        color1 = color2;
        ++stop2;
        if (stop2 != stops.end())
          color2 = stopColor(*stop2, opacity);
      }
      if (stop2 == stops.begin() || stop2 == stops.end())
        colors_[i] = color1;
      else
        colors_[i] = color1.gradient(color2, (offset - stop1->offset_) / (stop2->offset_ - stop1->offset_));
    }
  }

  static unsigned size() { return size_; }
  const agg::rgba8 & operator[] (unsigned v) const
  {
    return colors_[v];
  }

private:
  static agg::rgba8 stopColor(GradientStop const & stop, double opacity)
  {
    if (opacity < 0.999)
    {
      agg::rgba8 color = stop.color_;
      return color.opacity(opacity * color.opacity());
    }
    return stop.color_;
  }

  agg::rgba8 colors_[size_];
};

template<class GradientFunc, class VertexSource>
void RenderScanlinesGradient(renderer_base_amask_t & renderer, 
  agg::rasterizer_scanline_aa<> & rasterizer,
  GradientFunc const & gradient_func, GradientBase const & gradient_base, 
  agg::trans_affine const & user_transform, agg::trans_affine const & gradient_geometry_transform,
  double opacity,
  VertexSource & curved)
{
  typedef agg::span_interpolator_linear<> span_interpolator_t;
  typedef GradientRepeatAdapter<GradientFunc> gradient_t;
  typedef agg::span_gradient< 
    agg::rgba8,
    span_interpolator_t,
    gradient_t,
    ColorFunctionProfile > span_gradient_t;
  typedef agg::span_allocator<typename span_gradient_t::color_type> span_allocator_t;

  static const double GradientScale = 100.0;
  agg::trans_affine tr = agg::trans_affine_scaling(1.0/GradientScale) * gradient_geometry_transform;

  if (gradient_base.matrix_)
    tr *= agg::trans_affine(gradient_base.matrix_->data());

  if (gradient_base.useObjectBoundingBox_)
  {
    double min_x, min_y, max_x, max_y;
    agg::bounding_rect_single(curved, 0, &min_x, &min_y, &max_x, &max_y);
    if (min_x >= max_x || min_y >= max_y)
      return;
    else
      tr *= agg::trans_affine(max_x - min_x, 0, 0, max_y - min_y, min_x, min_y);
  }

  tr *= user_transform;
  tr.invert();
  span_interpolator_t span_interpolator(tr);
  ColorFunctionProfile color_function(gradient_base.stops_, opacity);
  gradient_t gradient_repeated(gradient_func, gradient_base.spreadMethod_);
  span_gradient_t span_gradient(span_interpolator, gradient_repeated, color_function, 0, GradientScale);
  span_allocator_t span_allocator;
  agg::scanline_p8 scanline;
  agg::render_scanlines_aa(rasterizer, scanline, renderer, span_allocator, span_gradient);
}

template<class VertexSource>
void Path::paintScanlines(EffectivePaint const & paint, double opacity, agg::rasterizer_scanline_aa<> & rasterizer,
  VertexSource & curved) 
{
  pixfmt_amask_adaptor_t amask_adaptor(getPixfmt(), clipBuffer().alphaMask());
  renderer_base_amask_t renderer_base(amask_adaptor);
  // TODO: pass bounding box function instead of curved
  if (agg::rgba8 const * paintColor = boost::get<agg::rgba8>(&paint))
  {
    agg::rgba8 color(*paintColor);
    color.opacity(opacity);
    typedef agg::renderer_scanline_aa_solid<renderer_base_amask_t> renderer_solid_t;
    renderer_solid_t renderer_solid(renderer_base);
    renderer_solid.color(color);
    agg::scanline_p8 scanline;
    agg::render_scanlines(rasterizer, scanline, renderer_solid);
  }
  else
  {
    Gradient const & gradient = boost::get<Gradient const>(paint);
    if (LinearGradient const * linearGradient = boost::get<LinearGradient>(&gradient))
    {
      agg::gradient_x gradient_func;
      double dx = linearGradient->x2_ - linearGradient->x1_;
      double dy = linearGradient->y2_ - linearGradient->y1_;
      agg::trans_affine gradient_geometry_transform = 
        agg::trans_affine_scaling(std::sqrt(dx * dx + dy * dy))
        * agg::trans_affine_rotation(std::atan2(dy, dx))
        * agg::trans_affine_translation(linearGradient->x1_, linearGradient->y1_);
      RenderScanlinesGradient(renderer_base, rasterizer,
        gradient_func, *linearGradient, transform(), gradient_geometry_transform, opacity, curved);
    }
    else
    {
      RadialGradient const & radialGradient = boost::get<RadialGradient>(gradient);
      agg::gradient_radial_focus gradient_func(radialGradient.r_, 
        radialGradient.fx_ - radialGradient.cx_, radialGradient.fy_ - radialGradient.cy_);
      agg::trans_affine gradient_geometry_transform = 
        agg::trans_affine_translation(radialGradient.cx_, radialGradient.cy_);
      RenderScanlinesGradient(renderer_base, rasterizer,
        gradient_func, radialGradient, transform(), gradient_geometry_transform, opacity, curved);
    }
  }
}

template<class VertexSourceStroked, class VertexSourceCurved>
void Path::strokePath(EffectivePaint const & stroke, 
  VertexSourceStroked & curved_stroked, VertexSourceCurved & curved) 
{
  curved_stroked.width(style().stroke_width_);
  curved_stroked.line_join(style().line_join_);
  curved_stroked.line_cap(style().line_cap_);
  curved_stroked.miter_limit(style().miterlimit_);
  curved_stroked.inner_join(agg::inner_round);
  //curved_stroked.approximation_scale(scl);

  // If the *visual* line width is considerable we 
  // turn on processing of curve cusps.
  //---------------------
  /*if(attr.stroke_width * scl > 1.0)
  {
      m_curved.angle_tolerance(0.2);
  }*/

  typedef agg::conv_transform<VertexSourceStroked> transformed_t;
  transformed_t curved_stroked_transformed(curved_stroked, transform());
  agg::rasterizer_scanline_aa<> rasterizer;
  rasterizer.filling_rule(agg::fill_non_zero);
  rasterizer.add_path(curved_stroked_transformed);
  paintScanlines(stroke, style().stroke_opacity_, rasterizer, curved);
}

void Path::drawPath()
{
  typedef agg::conv_curve<agg::path_storage> curved_t;
  typedef agg::conv_transform<curved_t> curved_transformed_t;
  typedef agg::conv_contour<curved_transformed_t> curved_transformed_contour_t;

  curved_t curved(path_storage_);

  path_storage_.arrange_orientations_all_paths(agg::path_flags_ccw); // TODO: move out
  
  EffectivePaint fill = getEffectivePaint(style().fill_paint_);
  if (boost::get<svgpp::tag::value::none>(&fill) == NULL)
  {
    curved_transformed_t curved_transformed(curved, transform());
    agg::rasterizer_scanline_aa<> rasterizer;
    rasterizer.filling_rule(style().nonzero_fill_rule_ ? agg::fill_non_zero : agg::fill_even_odd);
    //if(fabs(m_curved_trans_contour.width()) < 0.0001)
    {
        rasterizer.add_path(curved_transformed);
    }
    /*else
    {
        m_curved_trans_contour.miter_limit(attr.miter_limit);
        ras.add_path(m_curved_trans_contour, attr.index);
    }*/

    paintScanlines(fill, style().fill_opacity_, rasterizer, curved);
  }

  EffectivePaint stroke = getEffectivePaint(style().stroke_paint_);
  if (boost::get<svgpp::tag::value::none>(&stroke) == NULL)
  {
    if (std::accumulate(style().stroke_dasharray_.begin(), style().stroke_dasharray_.end(), 0.0) <= 0.0)
    {
      typedef agg::conv_stroke<curved_t> curved_stroked_t;
      curved_stroked_t curved_stroked(curved);
      strokePath(stroke, curved_stroked, curved);
    }
    else
    {
      typedef agg::conv_dash<curved_t> curved_dashed_t;
      curved_dashed_t curved_dashed(curved);

      std::vector<double> const & dasharray = style().stroke_dasharray_;
      int num_dash_values = 
        dasharray.size() % 2 == 0
          ? dasharray.size() 
          : 2 * dasharray.size();
      for(int i=0; i<num_dash_values; i+=2)
        curved_dashed.add_dash(dasharray[i % dasharray.size()], dasharray[(i+1) % dasharray.size()]);

      curved_dashed.dash_start(style().stroke_dashoffset_);

      typedef agg::conv_stroke<curved_dashed_t> curved_stroked_t;
      curved_stroked_t curved_stroked(curved_dashed);
      strokePath(stroke, curved_stroked, curved);
    }
  }
}

class Marker: 
  public Canvas
{
public:
  Marker(Path & parent, double strokeWidth, double x, double y, double autoOrient)
    : Canvas(parent, dontInheritStyle())
    , autoOrient_(autoOrient)
    , strokeWidth_(strokeWidth)
    , refX_(0)
    , refY_(0)
    , orient_(0.0)
    , strokeWidthUnits_(true)
  {
    transform().premultiply(agg::trans_affine_translation(x, y));
  }

  void on_enter_element(svgpp::tag::element::marker) {}
  void on_exit_element() {}

  bool notify(afterMarkerUnitsTag)
  {
    // strokeWidthUnits_ and orient_ already set
    if (strokeWidthUnits_)
    {
      length_factory() = length_factory_t();
      transform().premultiply(agg::trans_affine_scaling(strokeWidth_));
    }
    transform().premultiply(agg::trans_affine_rotation(orient_));
    return true;
  }

  using Canvas::set;

  void set(svgpp::tag::attribute::markerUnits, svgpp::tag::value::strokeWidth)
  { strokeWidthUnits_ = true; }

  void set(svgpp::tag::attribute::markerUnits, svgpp::tag::value::userSpaceOnUse)
  { strokeWidthUnits_ = false; }

  void set(svgpp::tag::attribute::refX, double val)
  { refX_ = val; }

  void set(svgpp::tag::attribute::refY, double val)
  { refY_ = val; }

  void set(svgpp::tag::attribute::orient, double val)
  { orient_ = val * boost::math::constants::degree<double>(); }

  void set(svgpp::tag::attribute::orient, svgpp::tag::value::auto_)
  { orient_ = autoOrient_; }

private:
  double const strokeWidth_;
  double const autoOrient_;
  bool strokeWidthUnits_;
  double refX_, refY_;
  double orient_;
};

void Path::drawMarkers()
{
  if (!style().marker_start_ && !style().marker_mid_ && !style().marker_end_)
    return;
  for(Markers::const_iterator pos = markers_.begin(); pos != markers_.end(); ++pos)
  {
    if (boost::optional<svg_string_t> & m = getMarkerReference(pos->v))
    {
      drawMarker(*m, pos->x, pos->y, pos->directionality);
    }
  }
}

void Path::drawMarker(svg_string_t const & id, double x, double y, double dir)
{
  if (XMLElement element = document().xml_document_.findElementById(id))
  {
    Document::FollowRef lock(document(), element);

    Marker markerContext(*this, style().stroke_width_, x, y, dir);
    document_traversal_main::load_referenced_element<
      svgpp::expected_elements<boost::mpl::set1<svgpp::tag::element::marker> >
    >::load(element, markerContext);
  }
}

struct GradientBase_visitor: boost::static_visitor<>
{
  void operator()(GradientBase const & g) 
  {
    gradient_ = &g;
  }

  GradientBase const * gradient_;
};

Path::EffectivePaint Path::getEffectivePaint(Paint const & paint) const
{
  SolidPaint const * solidPaint = NULL;
  if (IRIPaint const * iri = boost::get<IRIPaint>(&paint))
  {
    if (boost::optional<Gradient> const gradient = document().gradients_.get(iri->fragment_, length_factory()))
    {
      GradientBase_visitor gradientBase;
      boost::apply_visitor(gradientBase, *gradient);
      if (gradientBase.gradient_->stops_.empty())
        return svgpp::tag::value::none();
      if (gradientBase.gradient_->stops_.size() == 1)
        return gradientBase.gradient_->stops_.front().color_;
      if (LinearGradient const * linearGradient = boost::get<LinearGradient>(gradient.get_ptr()))
      {
        if (linearGradient->x1_ == linearGradient->x2_ && linearGradient->y1_ == linearGradient->y2_)
          // TODO: use also last step opacity 
          return gradientBase.gradient_->stops_.back().color_;
      }
      return *gradient;
    }
    if (iri->fallback_)
      solidPaint = &*iri->fallback_;
    else
      throw std::runtime_error("Can't find paint server");
  }
  else
    solidPaint = boost::get<SolidPaint>(&paint);
  if (boost::get<svgpp::tag::value::none>(solidPaint))
    return svgpp::tag::value::none();
  if (boost::get<svgpp::tag::value::currentColor>(solidPaint))
    return style().color_;
  return boost::get<agg::rgba8>(*solidPaint);
}

void renderDocument(XMLDocument & xmlDocument, ImageBuffer & buffer)
{
  Document document(xmlDocument);
  Canvas canvas(document, buffer);
  document_traversal_main::load_document(xmlDocument.getRoot(), canvas);
}

int main(int argc, char * argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " <svg file name> [<output BMP file name>]\n";
    return 1;
  }
  ImageBuffer buffer;
  
  try
  {
    XMLDocument xmlDoc;
    xmlDoc.load(argv[1]);
    renderDocument(xmlDoc, buffer);
  }
  catch(std::exception const & e)
  {
    std::cerr << "Error reading file " << argv[1] << ": " << e.what() << "\n";
    return 1;
  }

  // Saving output
  std::ofstream file(argc > 2 ? argv[2] : "svgpp.bmp", std::ios::out | std::ios::binary);
  if (file)
  {
    bmp::write_32bit_header(file, buffer.pixfmt().width(), buffer.pixfmt().height());
    file.write(reinterpret_cast<const char *>(buffer.pixfmt().row_ptr(0)), 
      buffer.pixfmt().width() * buffer.pixfmt().height() * 4);
  }
  else
  {
    std::cerr << "Can't open file for writing\n";
    return 1;
  }
  return 0;
}