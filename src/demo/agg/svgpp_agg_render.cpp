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

#include <boost/bind.hpp>
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
#include <agg_span_allocator.h>
#include <agg_span_gradient.h>
#include <agg_span_interpolator_linear.h>

#include <map>
#include <set>
#include <fstream>

#include "bmp_header.hpp"
#include "stylable.hpp"
#include "gradient.hpp"

class Canvas;
class Path;
class Use;

typedef agg::pixfmt_rgba32 pixfmt_t;
typedef agg::renderer_base<pixfmt_t> renderer_base_t;

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
  struct apply<Canvas, svgpp::tag::element::use_, void>
  {
    typedef svgpp::context_factory::on_stack<Canvas, Use> type;
  };

  template<class ElementTag>
  struct apply<Canvas, ElementTag, typename boost::enable_if<boost::mpl::has_key<svgpp::traits::shape_elements, ElementTag> >::type>
  {
    typedef svgpp::context_factory::on_stack<Canvas, Path> type;
  };
};

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
    svgpp::tag::attribute::stroke,
    svgpp::tag::attribute::stroke_width,
    svgpp::tag::attribute::fill,
    svgpp::tag::attribute::stroke_opacity,
    svgpp::tag::attribute::fill_opacity,
    svgpp::tag::attribute::fill_rule,
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

typedef 
  svgpp::document_traversal<
    svgpp::context_factories<child_context_factories>,
    svgpp::color_factory<color_factory>,
    svgpp::processed_elements<boost::mpl::set<
      svgpp::tag::element::svg,
      svgpp::tag::element::g,
      svgpp::tag::element::a,
      svgpp::tag::element::use_,
      svgpp::tag::element::path,
      svgpp::tag::element::rect,
      svgpp::tag::element::line,
      svgpp::tag::element::circle,
      svgpp::tag::element::ellipse,
      svgpp::tag::element::polyline
    > >,
    svgpp::processed_attributes<processed_attributes>,
    svgpp::basic_shapes_policy<basic_shapes_policy>,
    svgpp::path_policy<path_policy>
  > document_traversal_main;

class ImageBuffer: boost::noncopyable
{
public:
  ImageBuffer(int width, int height)
    : buffer_(width * height * pixfmt_t::pix_width)
    , rbuf_(&buffer_[0], width, height, width * pixfmt_t::pix_width)
    , pixfmt_(rbuf_)
    , renderer_base_(pixfmt_)
  {}

  pixfmt_t & pixfmt() { return pixfmt_; }
  renderer_base_t & renderer_base() { return renderer_base_; }

private:
  std::vector<unsigned char> buffer_;
  agg::rendering_buffer rbuf_;
  pixfmt_t pixfmt_;
  renderer_base_t renderer_base_;
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

typedef boost::function<renderer_base_t&()> lazy_renderer_t;

class Canvas: 
  public Stylable,
  public Transformable
{
public:
  Canvas(Document & document, ImageBuffer & image_buffer)
    : document_(document)
    , parent_renderer_(boost::bind(&ImageBuffer::renderer_base, boost::ref(image_buffer)))
  {}

  Canvas(Canvas & parent)
    : Transformable(parent)
    , Stylable(parent)
    , document_(parent.document_)
    , parent_renderer_(boost::bind(&Canvas::get_renderer, &parent))
  {}

  void on_exit_element()
  {
    if (own_buffer_.get())
      parent_renderer_().blend_from(own_buffer_->pixfmt(), NULL, 0, 0, unsigned(style().opacity_ * 255));
  }

private:
  Document & document_;
  lazy_renderer_t parent_renderer_;
  std::auto_ptr<ImageBuffer> own_buffer_;

protected:
  renderer_base_t & get_renderer()
  {
    renderer_base_t & parent_renderer = parent_renderer_();

    if (style().opacity_ < 0.999)
    {
      if (!own_buffer_.get())
        own_buffer_.reset(new ImageBuffer(parent_renderer.width(), parent_renderer.height()));
      return own_buffer_->renderer_base();
    }
    return parent_renderer;
  }

  Document & document() const { return document_; }
};

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
    if (XMLElement element = document().xml_document_.find_element_by_id(fragment_id_))
    {
      Document::FollowRef lock(document(), element);
      transform().premultiply(agg::trans_affine_translation(x_, y_));
      document_traversal_main::load_referenced_element<svgpp::traits::reusable_elements>(element, static_cast<Canvas&>(*this));
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

private:
  svg_string_t fragment_id_;
  double x_, y_;
};

class Path: public Canvas
{
public:
  Path(Canvas & parent)
    : Canvas(parent)
  {}

  void on_exit_element()
  {
    if (path_storage_.total_vertices() > 0)
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

private:
  agg::path_storage path_storage_;

  typedef boost::variant<svgpp::tag::value::none, agg::rgba8, Gradient const *> EffectivePaint;
  template<class VertexSource>
  void paint_scanlines(EffectivePaint const & paint, double opacity, agg::rasterizer_scanline_aa<> & rasterizer,
    VertexSource & curved);
  void draw_path();
  EffectivePaint get_effective_paint(Paint const &) const;
};

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
void render_scanlines_gradient(renderer_base_t & renderer, 
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
  // TODO: pass bounding box function instead of curved
  if (agg::rgba8 const * paintColor = boost::get<agg::rgba8>(&paint))
  {
    agg::rgba8 color(*paintColor);
    color.opacity(opacity);
    typedef agg::renderer_scanline_aa_solid<renderer_base_t> renderer_solid_t;
    renderer_solid_t renderer_solid(get_renderer());
    renderer_solid.color(color);
    agg::scanline_p8 scanline;
    agg::render_scanlines(rasterizer, scanline, renderer_solid);
  }
  else
  {
    Gradient const & gradient = *boost::get<Gradient const *>(paint);
    if (LinearGradient const * linearGradient = boost::get<LinearGradient>(&gradient))
    {
      agg::gradient_x gradient_func;
      double dx = linearGradient->x2_ - linearGradient->x1_;
      double dy = linearGradient->y2_ - linearGradient->y1_;
      agg::trans_affine gradient_geometry_transform = 
        agg::trans_affine_scaling(std::sqrt(dx * dx + dy * dy))
        * agg::trans_affine_rotation(std::atan2(dy, dx))
        * agg::trans_affine_translation(linearGradient->x1_, linearGradient->y1_);
      render_scanlines_gradient(get_renderer(), rasterizer,
        gradient_func, *linearGradient, transform(), gradient_geometry_transform, opacity, curved);
    }
    else
    {
      RadialGradient const & radialGradient = boost::get<RadialGradient>(gradient);
      agg::gradient_radial_focus gradient_func(radialGradient.r_, 
        radialGradient.fx_ - radialGradient.cx_, radialGradient.fy_ - radialGradient.cy_);
      agg::trans_affine gradient_geometry_transform = 
        agg::trans_affine_translation(radialGradient.cx_, radialGradient.cy_);
      render_scanlines_gradient(get_renderer(), rasterizer,
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
    if (Gradient const * gradient = document().gradients_.get(iri->fragment_))
    {
      GradientBase_visitor gradientBase;
      boost::apply_visitor(gradientBase, *gradient);
      if (gradientBase.gradient_->stops_.empty())
        return svgpp::tag::value::none();
      if (gradientBase.gradient_->stops_.size() == 1)
        return gradientBase.gradient_->stops_.front().color_;
      if (LinearGradient const * linearGradient = boost::get<LinearGradient>(gradient))
      {
        if (linearGradient->x1_ == linearGradient->x2_ && linearGradient->y1_ == linearGradient->y2_)
          // TODO: use also last step opacity 
          return gradientBase.gradient_->stops_.back().color_;
      }
      return gradient;
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
  buffer.renderer_base().clear(pixfmt_t::color_type(255, 255, 255));

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
  static const int frame_width = 1000, frame_height = 1000;
  ImageBuffer buffer(frame_width, frame_height);

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