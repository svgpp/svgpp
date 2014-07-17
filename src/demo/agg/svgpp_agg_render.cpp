#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_SET_SIZE 40

#include "common.hpp"

#ifdef USE_MSXML
# include <comip.h>
# include <comdef.h>
#else
# include <rapidxml_ns/rapidxml_ns_utils.hpp>
#endif

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
#include <agg_path_storage.h>
#include <agg_pixfmt_amask_adaptor.h>
#include <agg_span_allocator.h>
#include <agg_span_gradient.h>
#include <agg_span_interpolator_linear.h>

#include <map>
#include <set>
#include <fstream>

#include "bmp_header.hpp"
#include "stylable.hpp"
#include "gradient.hpp"
#include "clip_path.hpp"

class Transformable;
class Canvas;
class Path;
class Use;
class Switch;
class ReferencedSymbolOrSvg;
class Mask;

typedef agg::pixfmt_rgba32 pixfmt_t;
typedef agg::pixfmt_amask_adaptor<pixfmt_t, ClipBuffer::alpha_mask_t> pixfmt_amask_adaptor_t;
typedef agg::renderer_base<pixfmt_amask_adaptor_t> renderer_base_amask_t;

#ifndef USE_MSXML
namespace
{
  rapidxml_ns::xml_node<> const * find_child_element_by_id(rapidxml_ns::xml_node<> const * parent, std::string const & id)
  {
    for(rapidxml_ns::xml_node<> const * node = parent->first_node(); node; node = node->next_sibling())
    {
      if (rapidxml_ns::xml_attribute<> const * id_attr = node->first_attribute("id"))
        if (boost::range::equal(boost::iterator_range<const char *>(id_attr->value(), id_attr->value() + id_attr->value_size()), id))
          return node;
      if (rapidxml_ns::xml_node<> const * child_node = find_child_element_by_id(node, id))
        return child_node;
    }
    return NULL;
  }
}

XMLElement XMLDocument::find_element_by_id(svg_string_t const & id)
{
  std::pair<element_by_id_t::iterator, bool> ins = element_by_id_.insert(element_by_id_t::value_type(id, NULL));
  if (ins.second)
    ins.first->second = find_child_element_by_id(root_, id);
  return ins.first->second;
}

#else

XMLElement XMLDocument::find_element_by_id(svg_string_t const & id)
{
  std::wstring xpath = L"//*[@id='" + id + L"']"; // TODO: escape id string, check namespace
  _com_ptr_t<_com_IIID<IXMLDOMNode, &IID_IXMLDOMNode> > node;
  if (S_OK != root_->selectSingleNode(_bstr_t(xpath.c_str()), &node))
    return XMLElement();
  return XMLElement(node.GetInterfacePtr());
}

#endif

struct Document
{
  class FollowRef;

  Document(XMLElement const & xml_root)
    : xml_document_(xml_root)
    , gradients_(xml_document_)
  {}

  XMLDocument xml_document_;
  Gradients gradients_;
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

struct basic_shapes_policy
{
  typedef boost::mpl::set<
    svgpp::tag::element::rect,
    svgpp::tag::element::line,
    svgpp::tag::element::circle,
    svgpp::tag::element::ellipse
  > convert_to_path;

  typedef boost::mpl::set<
  > collect_attributes;

  static const bool convert_only_rounded_rect_to_path = false;
  static const bool viewport_as_transform = true;
  static const bool calculate_viewport = true;
  static const bool polyline_as_path = true;
};

struct child_context_factories
{
  template<class ParentContext, class ElementTag, class Enable = void>
  struct apply;

  template<>
  struct apply<Canvas, svgpp::tag::element::svg, void>
  {
    typedef svgpp::context_factory::on_stack<Canvas, Canvas> type;
  };

  template<>
  struct apply<Canvas, svgpp::tag::element::g, void>
  {
    typedef svgpp::context_factory::on_stack<Canvas, Canvas> type;
  };

  template<>
  struct apply<Canvas, svgpp::tag::element::a, void>
  {
    typedef svgpp::context_factory::on_stack<Canvas, Canvas> type;
  };

  template<>
  struct apply<Canvas, svgpp::tag::element::switch_, void>
  {
    typedef svgpp::context_factory::on_stack<Canvas, Switch> type;
  };

  template<class ElementTag>
  struct apply<Switch, ElementTag, void>: apply<Canvas, ElementTag>
  {};

  template<>
  struct apply<Canvas, svgpp::tag::element::use_, void>
  {
    typedef svgpp::context_factory::on_stack<Canvas, Use> type;
  };

  template<class ElementTag>
  struct apply<Canvas, ElementTag, typename boost::enable_if<boost::mpl::has_key<svgpp::traits::shape_elements, ElementTag> >::type>
  {
    typedef svgpp::context_factory::on_stack<Canvas, Path> type;
  };

  // For referenced by 'use' elements
  template<>
  struct apply<Use, svgpp::tag::element::svg, void>
  {
    typedef svgpp::context_factory::on_stack<Use, ReferencedSymbolOrSvg> type;
  };

  template<>
  struct apply<Use, svgpp::tag::element::symbol, void>
  {
    typedef svgpp::context_factory::on_stack<Use, ReferencedSymbolOrSvg> type;
  };

  template<class ElementTag>
  struct apply<Use, ElementTag, void>: apply<Canvas, ElementTag>
  {};

  template<class ElementTag>
  struct apply<ReferencedSymbolOrSvg, ElementTag, void>: apply<Canvas, ElementTag>
  {};

  // 'mask'
  template<>
  struct apply<Mask, svgpp::tag::element::mask, void>
  {
    typedef svgpp::context_factory::same<Mask, svgpp::tag::element::mask> type;
  };

  template<class ElementTag>
  struct apply<Mask, ElementTag, void>: apply<Canvas, ElementTag>
  {};
};

struct document_traversal_control
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
  svgpp::tag::element::ellipse
  //svgpp::tag::element::polyline
> processed_elements;

typedef boost::mpl::fold<
  boost::mpl::protect<
    boost::mpl::joint_view<
      boost::mpl::transform_view<
        svgpp::rect_shape_attributes, boost::mpl::pair<svgpp::tag::element::rect, boost::mpl::_1> >,
      boost::mpl::transform_view<
          svgpp::traits::viewport_attributes, boost::mpl::pair<svgpp::tag::element::svg, boost::mpl::_1> >
    >
  >,
  boost::mpl::set<
    //svgpp::tag::attribute::style, TODO:
    svgpp::tag::attribute::display,
    svgpp::tag::attribute::d,
    svgpp::tag::attribute::transform,
    svgpp::tag::attribute::points,
    svgpp::tag::attribute::x1,
    svgpp::tag::attribute::y1,
    svgpp::tag::attribute::x2,
    svgpp::tag::attribute::y2,
    svgpp::tag::attribute::cx,
    svgpp::tag::attribute::cy,
    svgpp::tag::attribute::r,
    svgpp::tag::attribute::rx,
    svgpp::tag::attribute::ry,
    svgpp::tag::attribute::color,
    svgpp::tag::attribute::fill,
    svgpp::tag::attribute::fill_opacity,
    svgpp::tag::attribute::fill_rule,
    svgpp::tag::attribute::marker_start,
    svgpp::tag::attribute::marker_mid,
    svgpp::tag::attribute::marker_end,
    svgpp::tag::attribute::mask,
    svgpp::tag::attribute::maskUnits,
    svgpp::tag::attribute::maskContentUnits,
    svgpp::tag::attribute::stroke,
    svgpp::tag::attribute::stroke_width,
    svgpp::tag::attribute::stroke_opacity,
    svgpp::tag::attribute::stroke_linecap,
    svgpp::tag::attribute::stroke_linejoin,
    svgpp::tag::attribute::stroke_miterlimit,
    svgpp::tag::attribute::stroke_dasharray,
    svgpp::tag::attribute::stroke_dashoffset,
    svgpp::tag::attribute::opacity,
    boost::mpl::pair<svgpp::tag::element::use_, svgpp::tag::attribute::xlink::href>,
    boost::mpl::pair<svgpp::tag::element::use_, svgpp::tag::attribute::x>,
    boost::mpl::pair<svgpp::tag::element::use_, svgpp::tag::attribute::y>
  >,
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
  void set_size(int width, int height, pixfmt_t::color_type const & fill_color)
  {
    BOOST_ASSERT(buffer_.empty());
    buffer_.resize(width * height * pixfmt_t::pix_width);
    rbuf_.attach(&buffer_[0], width, height, width * pixfmt_t::pix_width);
    pixfmt_.attach(rbuf_);
    agg::renderer_base<pixfmt_t> renderer_base(pixfmt_);
    renderer_base.clear(fill_color);
  }

  boost::gil::rgba8_view_t gil_view()
  {
    return boost::gil::interleaved_view(rbuf_.width(), rbuf_.height(), 
      reinterpret_cast<boost::gil::rgba8_pixel_t*>(buffer_.data()), rbuf_.stride());
  }

private:
  std::vector<unsigned char> buffer_;
  agg::rendering_buffer rbuf_;
  pixfmt_t pixfmt_;
};

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
  Canvas(Document & document, ImageBuffer & image_buffer)
    : document_(document)
    , parent_pixfmt_(boost::bind(&ImageBuffer::pixfmt, boost::ref(image_buffer)))
    , image_buffer_(&image_buffer)
    , is_switch_child_(false)
  {}

  Canvas(Canvas & parent)
    : Transformable(parent)
    , Stylable(parent)
    , document_(parent.document_)
    , image_buffer_(NULL)
    , parent_pixfmt_(boost::bind(&Canvas::get_pixfmt, &parent))
    , is_switch_child_(parent.is_switch_element())
    , length_factory_(parent.length_factory_)
    , clip_buffer_(parent.clip_buffer_)
  {}

  void on_exit_element()
  {
    if (!own_buffer_.get())
      return;
    if (style().mask_fragment_)
    {
      pixfmt_t & parent_pixfmt = parent_pixfmt_();
      ImageBuffer mask_buffer(parent_pixfmt.width(), parent_pixfmt.height());
      load_mask(mask_buffer);
      auto mask_view = boost::gil::color_converted_view<boost::gil::gray8_pixel_t>(
        mask_buffer.gil_view(), svgpp::gil_utility::rgba_to_mask_color_converter<>());

      auto own_view = own_buffer_->gil_view();
      auto o = own_view.begin();
      for(auto m = mask_view.begin(); m !=mask_view.end(); ++m, ++o)
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
      image_buffer_->set_size(viewport_width + 1.0, viewport_height + 1.0, pixfmt_t::color_type(255, 255, 255, 0));
      clip_buffer_.reset(new ClipBuffer(image_buffer_->pixfmt().width(), image_buffer_->pixfmt().height()));
    }
    else
    {
      if (!clip_buffer_.unique())
        clip_buffer_.reset(new ClipBuffer(*clip_buffer_));
      clip_buffer_->intersect_clip_rect(transform(), viewport_x, viewport_y, viewport_width, viewport_height);
    }
    length_factory_.set_viewport_size(viewport_width, viewport_height);
  }

  length_factory_t const & length_factory() const
  {
    return length_factory_;
  }

private:
  Document & document_;
  ImageBuffer * const image_buffer_; // Non-NULL only for topmost SVG element
  lazy_pixfmt_t parent_pixfmt_;
  std::auto_ptr<ImageBuffer> own_buffer_;
  boost::shared_ptr<ClipBuffer> clip_buffer_;
  length_factory_t length_factory_;

  void load_mask(ImageBuffer &) const;

protected:
  pixfmt_t & get_pixfmt()
  {
    pixfmt_t & parent_pixfmt = parent_pixfmt_();

    if (style().opacity_ < 0.999 || style().mask_fragment_)
    {
      if (!own_buffer_.get())
        own_buffer_.reset(new ImageBuffer(parent_pixfmt.width(), parent_pixfmt.height()));
      return own_buffer_->pixfmt();
    }
    return parent_pixfmt;
  }

  Document & document() const { return document_; }
  ClipBuffer const & clip_buffer() const { return *clip_buffer_; }
  virtual bool is_switch_element() const { return false; }

  const bool is_switch_child_;
};

class Switch: public Canvas
{
public:
  Switch(Canvas & parent)
    : Canvas(parent)
  {}

  virtual bool is_switch_element() const { return true; }
};

class Path: public Canvas
{
public:
  Path(Canvas & parent)
    : Canvas(parent)
  {}

  void on_exit_element()
  {
    if (style().display_ && path_storage_.total_vertices() > 0)
      draw_path();
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
  {}

  void marker(svgpp::marker_vertex v, double x, double y, svgpp::tag::orient_fixed, unsigned marker_index)
  {}

  void marker_get_config(svgpp::marker_config & start, svgpp::marker_config & mid, svgpp::marker_config & end)
  {}

private:
  agg::path_storage path_storage_;

  typedef boost::variant<svgpp::tag::value::none, agg::rgba8, Gradient> EffectivePaint;
  template<class VertexSource>
  void paint_scanlines(EffectivePaint const & paint, double opacity, agg::rasterizer_scanline_aa<> & rasterizer,
    VertexSource & curved);
  void draw_path();
  EffectivePaint get_effective_paint(Paint const &) const;
};

typedef 
  svgpp::document_traversal<
    svgpp::context_factories<child_context_factories>,
    svgpp::length_policy<svgpp::policy::length::forward_to_method<Canvas, const length_factory_t> >,
    svgpp::color_factory<color_factory_t>,
    svgpp::processed_elements<processed_elements>,
    svgpp::processed_attributes<processed_attributes>,
    svgpp::basic_shapes_policy<basic_shapes_policy>,
    svgpp::path_policy<path_policy>,
    svgpp::document_traversal_control_policy<document_traversal_control>,
    svgpp::load_transform_policy<svgpp::policy::load_transform::forward_to_method<Transformable> >, // Same as default, but less instantiations
    svgpp::load_path_policy<svgpp::policy::load_path::forward_to_method<Path> >, // Same as default, but less instantiations
    svgpp::error_policy<svgpp::policy::error::default_policy<Stylable> >, // Type of context isn't used
    svgpp::markers_policy<svgpp::policy::markers::calculate>
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
    if (XMLElement element = document().xml_document_.find_element_by_id(fragment_id_))
    {
      Document::FollowRef lock(document(), element);
      transform().premultiply(agg::trans_affine_translation(x_, y_));
      document_traversal_main::load_referenced_element<
        svgpp::tag::element::use_,
        svgpp::traits::reusable_elements,
        svgpp::processed_elements<
          boost::mpl::insert<processed_elements, svgpp::tag::element::symbol>::type 
        >
      >(element, *this);
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
  { std::cerr << "External references aren't supported"; }

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

void Canvas::load_mask(ImageBuffer & mask_buffer) const
{
  if (XMLElement element = document().xml_document_.find_element_by_id(*style().mask_fragment_))
  {
    Document::FollowRef lock(document(), element);

    Mask mask(document_, mask_buffer, *this);
    document_traversal_main::load_referenced_element<
      svgpp::tag::attribute::mask,
      boost::mpl::set1<svgpp::tag::element::mask>,
      svgpp::processed_elements<boost::mpl::set1<svgpp::tag::element::mask> > 
    >(element, mask);
  }
  else
    throw std::runtime_error("Element referenced by 'mask' not found");
}

template<class Gradient>
class svg_gradient_repeat_adapter
{
public:
  svg_gradient_repeat_adapter(Gradient const & gradient, GradientBase::SpreadMethod method)
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

struct color_function_profile
{
  static const unsigned size_ = 256;

  color_function_profile(GradientStops const & stops, double opacity) 
  {
    assert(stops.size() >= 2);

    static const double offset_step = 1.0 / size_;
    double offset = 0;
    GradientStops::const_iterator stop1 = stops.begin(), stop2 = stops.begin();
    agg::rgba8 color1 = stop_color(*stop1, opacity), color2 = color1;
    for(int i = 0; i < size_; ++i, offset += offset_step)
    {
      while(offset > stop2->offset_ && stop2 != stops.end())
      {
        stop1 = stop2;
        color1 = color2;
        ++stop2;
        if (stop2 != stops.end())
          color2 = stop_color(*stop2, opacity);
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
  static agg::rgba8 stop_color(GradientStop const & stop, double opacity)
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
void render_scanlines_gradient(renderer_base_amask_t & renderer, 
  agg::rasterizer_scanline_aa<> & rasterizer,
  GradientFunc const & gradient_func, GradientBase const & gradient_base, 
  agg::trans_affine const & user_transform, agg::trans_affine const & gradient_geometry_transform,
  double opacity,
  VertexSource & curved)
{
  typedef agg::span_interpolator_linear<> span_interpolator_t;
  typedef svg_gradient_repeat_adapter<GradientFunc> gradient_t;
  typedef agg::span_gradient< 
    agg::rgba8,
    span_interpolator_t,
    gradient_t,
    color_function_profile > span_gradient_t;
  typedef agg::span_allocator<span_gradient_t::color_type> span_allocator_t;

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
  color_function_profile color_function(gradient_base.stops_, opacity);
  gradient_t gradient_repeated(gradient_func, gradient_base.spreadMethod_);
  span_gradient_t span_gradient(span_interpolator, gradient_repeated, color_function, 0, GradientScale);
  span_allocator_t span_allocator;
  agg::scanline_p8 scanline;
  agg::render_scanlines_aa(rasterizer, scanline, renderer, span_allocator, span_gradient);
}

template<class VertexSource>
void Path::paint_scanlines(EffectivePaint const & paint, double opacity, agg::rasterizer_scanline_aa<> & rasterizer,
  VertexSource & curved) 
{
  pixfmt_amask_adaptor_t amask_adaptor(get_pixfmt(), clip_buffer().alpha_mask());
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
      render_scanlines_gradient(renderer_base, rasterizer,
        gradient_func, *linearGradient, transform(), gradient_geometry_transform, opacity, curved);
    }
    else
    {
      RadialGradient const & radialGradient = boost::get<RadialGradient>(gradient);
      agg::gradient_radial_focus gradient_func(radialGradient.r_, 
        radialGradient.fx_ - radialGradient.cx_, radialGradient.fy_ - radialGradient.cy_);
      agg::trans_affine gradient_geometry_transform = 
        agg::trans_affine_translation(radialGradient.cx_, radialGradient.cy_);
      render_scanlines_gradient(renderer_base, rasterizer,
        gradient_func, radialGradient, transform(), gradient_geometry_transform, opacity, curved);
    }
  }
}

void Path::draw_path()
{
  typedef agg::conv_curve<agg::path_storage> curved_t;
  typedef agg::conv_transform<curved_t> curved_transformed_t;
  typedef agg::conv_contour<curved_transformed_t> curved_transformed_contour_t;

  curved_t curved(path_storage_);

  path_storage_.arrange_orientations_all_paths(agg::path_flags_ccw); // TODO: move out
  
  EffectivePaint fill = get_effective_paint(style().fill_paint_);
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

    paint_scanlines(fill, style().fill_opacity_, rasterizer, curved);
  }

  EffectivePaint stroke = get_effective_paint(style().stroke_paint_);
  if (boost::get<svgpp::tag::value::none>(&stroke) == NULL)
  {
    typedef agg::conv_stroke<curved_t> curved_stroked_t;
    typedef agg::conv_transform<curved_stroked_t> curved_stroked_transformed_t;

    curved_stroked_t curved_stroked(curved);
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

    curved_stroked_transformed_t curved_stroked_transformed(curved_stroked, transform());
    agg::rasterizer_scanline_aa<> rasterizer;
    rasterizer.filling_rule(agg::fill_non_zero);
    rasterizer.add_path(curved_stroked_transformed);
    paint_scanlines(stroke, style().stroke_opacity_, rasterizer, curved);
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

Path::EffectivePaint Path::get_effective_paint(Paint const & paint) const
{
  SolidPaint const * solidPaint = nullptr;
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

template<class XMLElement>
void render_document(XMLElement & svg_element, ImageBuffer & buffer)
{
  Document document(svg_element);
  Canvas canvas(document, buffer);
  document_traversal_main::load_document(svg_element, canvas);
}

int main(int argc, char * argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " <svg file name>\n";
    return 1;
  }
  ImageBuffer buffer;

#ifdef USE_MSXML
  
  //init
  HRESULT hr;
  if (FAILED(hr = CoInitialize(NULL)))
  {
    std::cerr << "CoInitialize failed with result 0x" << std::hex << hr << "\n" << std::dec;
    return 1;
  }
  BOOST_SCOPE_EXIT(void) {
    CoUninitialize();
  } BOOST_SCOPE_EXIT_END

  _com_ptr_t<_com_IIID<IXMLDOMDocument, &IID_IXMLDOMDocument> > docPtr;
  if (FAILED(hr = docPtr.CreateInstance(L"Msxml2.DOMDocument.3.0")))
  {
    std::cerr << "Error creating DOMDocument 0x" << std::hex << hr << "\n" << std::dec;
    return 1;
  }

  // Load a document.
  docPtr->put_async(VARIANT_FALSE);
  docPtr->put_resolveExternals(VARIANT_FALSE);
  docPtr->put_validateOnParse(VARIANT_FALSE);
  VARIANT_BOOL load_result;
  if (S_OK != (hr = docPtr->load(_variant_t(argv[1]), &load_result)))
  {
    _com_ptr_t<_com_IIID<IXMLDOMParseError, &IID_IXMLDOMParseError> > parseError;
    if (SUCCEEDED(docPtr->get_parseError(&parseError)))
    {
      _bstr_t reason;
      if (S_OK == parseError->get_reason(reason.GetAddress()))
      {
        std::cerr << "Parse error: " << std::string(reason.GetBSTR(), reason.GetBSTR() + reason.length()) << "\n";
        return 1;
      }
    }
    std::cerr << "Error loading XML document 0x" << std::hex << hr << "\n" << std::dec;
    return 1;
  }

  _com_ptr_t<_com_IIID<IXMLDOMElement, &IID_IXMLDOMElement> > root;
  if (FAILED(hr = docPtr->get_documentElement(&root)))
  {
    std::cerr << "Error getting documentElement 0x" << std::hex << hr << "\n" << std::dec;
    return 1;
  }

  try
  {
    render_document(root.GetInterfacePtr(), buffer);
  }
  catch(std::exception const & e)
  {
    std::cerr << "Error reading file " << argv[1] << ": " << e.what() << "\n";
    return 1;
  }

#else
  try
  {
    rapidxml_ns::file<> xml_file(argv[1]);
    rapidxml_ns::xml_document<> doc;    // character type defaults to char
    doc.parse<rapidxml_ns::parse_no_string_terminators>(xml_file.data());  
    if (rapidxml_ns::xml_node<> * svg_element = doc.first_node("svg"))
    {
      render_document(svg_element, buffer);
    }
    else
    {
      std::cerr << "Can't find root <svg> element\n";
      return 1;
    }
  }
  catch(rapidxml_ns::parse_error const & e)
  {
    std::cerr << "Error parsing file " << argv[1] << ": " << e.what() << "\n"
      "Invalid or unsupported XML\n";
    return 1;
  }
  catch(std::exception const & e)
  {
    std::cerr << "Error reading file " << argv[1] << ": " << e.what() << "\n";
    return 1;
  }
#endif

  // Saving output
  std::ofstream file("svgpp.bmp", std::ios::out | std::ios::binary);
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