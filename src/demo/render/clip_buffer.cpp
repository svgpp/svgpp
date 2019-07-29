#include "clip_buffer.hpp"

#if defined(RENDERER_AGG)
#include <agg_alpha_mask_u8.h>
#include <agg_conv_curve.h>
#include <agg_path_storage.h>
#include <agg_pixfmt_amask_adaptor.h>
#include <agg_pixfmt_gray.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_renderer_base.h>
#include <agg_renderer_scanline.h>
#include <agg_scanline_p.h>
#endif

#include <svgpp/svgpp.hpp>

#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/typedefs.hpp>

#if defined(RENDERER_AGG)
typedef agg::pixfmt_gray8 pixfmt_t;
typedef agg::renderer_base<pixfmt_t> renderer_base_t;
#endif

ClipBuffer::ClipBuffer(int width, int height)
  : buffer_(width * height, 0xff)
  , width_(width), height_(height)
{}

ClipBuffer::ClipBuffer(ClipBuffer const & src)
  : buffer_(src.buffer_)
  , width_(src.width_), height_(src.height_)
{}

boost::gil::gray8c_view_t ClipBuffer::gilView() const
{
  return boost::gil::interleaved_view(width_, height_, 
    reinterpret_cast<const boost::gil::gray8_pixel_t *>(&buffer_[0]), width_);
}

void ClipBuffer::intersectClipRect(transform_t const & transform, number_t x, number_t y, number_t width, number_t height)
{
#if defined(RENDERER_AGG)
  typedef agg::renderer_scanline_aa_solid<renderer_base_t> renderer_t;

  agg::rendering_buffer rbuf(&buffer_[0], width_, height_, width_);
  pixfmt_t pixfmt(rbuf);
  renderer_base_t renderer_base(pixfmt);
  agg::scanline_p8 scanline;

  agg::rasterizer_scanline_aa<> rasterizer;
  rasterizer.filling_rule(agg::fill_even_odd);

  rasterizer.move_to_d(0, 0);
  rasterizer.line_to_d(pixfmt.width(), 0);
  rasterizer.line_to_d(pixfmt.width(), pixfmt.height());
  rasterizer.line_to_d(0, pixfmt.height());
  rasterizer.close_polygon();

  number_t px = x, py = y;
  transform.transform(&px, &py);
  rasterizer.move_to_d(px, py);
  px = x + width; py = y;
  transform.transform(&px, &py);
  rasterizer.line_to_d(px, py);
  px = x + width; py = y + height;
  transform.transform(&px, &py);
  rasterizer.line_to_d(px, py);
  px = x; py = y + height;
  transform.transform(&px, &py);
  rasterizer.line_to_d(px, py);
  rasterizer.close_polygon();

  agg::render_scanlines_aa_solid(rasterizer, scanline, renderer_base, agg::gray8(0));
#elif defined(RENDERER_GDIPLUS)

#endif
}

namespace 
{
  typedef boost::mpl::insert<
    svgpp::traits::shape_elements,
    svgpp::tag::element::use_
  >::type processed_elements;

#if defined(RENDERER_GDIPLUS)
  struct path_policy: svgpp::policy::path::no_shorthands
  {
    static const bool arc_as_cubic_bezier = true; 
    static const bool quadratic_bezier_as_cubic = true;
  };
#else
  struct path_policy: svgpp::policy::path::no_shorthands
  {
    static const bool arc_as_cubic_bezier = true; 
  };
#endif

  typedef boost::mpl::fold<
    svgpp::traits::shapes_attributes_by_element,
    boost::mpl::set<
      svgpp::tag::attribute::clip_rule,
      svgpp::tag::attribute::display,
      svgpp::tag::attribute::transform,
	    boost::mpl::pair<svgpp::tag::element::use_, svgpp::tag::attribute::xlink::href>,
	    boost::mpl::pair<svgpp::tag::element::use_, svgpp::tag::attribute::x>,
	    boost::mpl::pair<svgpp::tag::element::use_, svgpp::tag::attribute::y>
    >::type,
    boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
  >::type processed_attributes;

  class ElementBase
  {
  public:
    ElementBase(
      XMLDocument & xml_document,
#if defined(RENDERER_AGG)
      agg::rendering_buffer & rbuf, 
#endif
      transform_t const & transform
      )
      : xml_document_(xml_document)
#if defined(RENDERER_AGG)
      , rbuf_(rbuf)
      , transform_(transform)
#endif
      , display_(true)
      , nonzero_clip_rule_(true)
    {
#if defined(RENDERER_GDIPLUS)
      AssignMatrix(transform_, transform);
#endif
    }

    ElementBase(ElementBase const & parent)
      : xml_document_(parent.xml_document_)
#if defined(RENDERER_AGG)
      , rbuf_(parent.rbuf_)
      , transform_(parent.transform_)
#endif
      , display_(parent.display_)
      , nonzero_clip_rule_(parent.nonzero_clip_rule_)
    {
#if defined(RENDERER_GDIPLUS)
      AssignMatrix(transform_, parent.transform_);
#endif
    }

    void set(svgpp::tag::attribute::display, svgpp::tag::value::none)
    { display_ = false; }

    void set(svgpp::tag::attribute::display, svgpp::tag::value::inherit)
    {}

    template<class ValueTag>
    void set(svgpp::tag::attribute::display, ValueTag)
    { display_ = true; }

    void set(svgpp::tag::attribute::clip_rule, svgpp::tag::value::nonzero)
    { nonzero_clip_rule_ = true; }

    void set(svgpp::tag::attribute::clip_rule, svgpp::tag::value::evenodd)
    { nonzero_clip_rule_ = false; }

    void transform_matrix(const boost::array<number_t, 6> & matrix)
    {
#if defined(RENDERER_AGG)
      transform_.premultiply(agg::trans_affine(matrix.data()));
#elif defined(RENDERER_GDIPLUS)
      transform_.Multiply(&Gdiplus::Matrix(matrix[0], matrix[2], matrix[1], matrix[3], matrix[4], matrix[5]));
#endif
    }

  protected:
    XMLDocument & xml_document_;
#if defined(RENDERER_AGG)
    agg::rendering_buffer & rbuf_;
#endif
    transform_t transform_;
    bool display_;
    bool nonzero_clip_rule_;
  };

  class Path: 
    public ElementBase
#if defined(RENDERER_GDIPLUS)
  , public PathStorage
#endif
  {
  public:
    Path(ElementBase const & parent)
      : ElementBase(parent)
    {}

#if defined(RENDERER_AGG)
    void path_move_to(number_t x, number_t y, svgpp::tag::coordinate::absolute const &)
    { 
      path_storage_.move_to(x, y);
    }

    void path_line_to(number_t x, number_t y, svgpp::tag::coordinate::absolute const &)
    { 
      path_storage_.line_to(x, y);
    }

    void path_cubic_bezier_to(
      number_t x1, number_t y1, 
      number_t x2, number_t y2, 
      number_t x, number_t y, 
      svgpp::tag::coordinate::absolute const &)
    { 
      path_storage_.curve4(x1, y1, x2, y2, x, y);
    }

    void path_quadratic_bezier_to(
      number_t x1, number_t y1, 
      number_t x, number_t y, 
      svgpp::tag::coordinate::absolute const &)
    { 
      path_storage_.curve3(x1, y1, x, y);
    }

    void path_close_subpath()
    {
      path_storage_.end_poly(agg::path_flags_close);
    }

    void path_exit()
    {}
#endif

    void on_exit_element()
    {
#if defined(RENDERER_AGG)
      if (display_ && path_storage_.total_vertices() > 0)
      {
        typedef agg::conv_curve<agg::path_storage> curved_t;
        typedef agg::conv_transform<curved_t> curved_transformed_t;

        curved_t curved(path_storage_);

        path_storage_.arrange_orientations_all_paths(agg::path_flags_ccw);
  
        curved_transformed_t curved_transformed(curved, transform_);
        agg::rasterizer_scanline_aa<> rasterizer;
        agg::scanline_p8 scanline;
        pixfmt_t pixfmt(rbuf_);
        renderer_base_t renderer_base(pixfmt);
        rasterizer.filling_rule(nonzero_clip_rule_ ? agg::fill_non_zero : agg::fill_even_odd);
        rasterizer.add_path(curved_transformed);
        agg::render_scanlines_aa_solid(rasterizer, scanline, renderer_base, agg::gray8(0));
      }
#elif defined(RENDERER_GDIPLUS)

#endif
    }

  private:
#if defined(RENDERER_AGG)
    agg::path_storage path_storage_;
#endif
  };

  class Use: public ElementBase
  {
  public:
    Use(ElementBase & parent)
      : ElementBase(parent)
    {}

    void on_exit_element();

    using ElementBase::set;

    template<class IRI>
    void set(svgpp::tag::attribute::xlink::href, svgpp::tag::iri_fragment, IRI const & fragment)
    { fragment_id_.assign(boost::begin(fragment), boost::end(fragment)); }

    template<class IRI>
    void set(svgpp::tag::attribute::xlink::href, IRI const & fragment)
    { std::cerr << "External references aren't supported\n"; }

    void set(svgpp::tag::attribute::x, number_t val)
    { x_ = val; }

    void set(svgpp::tag::attribute::y, number_t val)
    { y_ = val; }

  private:
    svg_string_t fragment_id_;
    number_t x_, y_;
  };

  struct context_factories
  {
    template<class ParentContext, class ElementTag>
    struct apply;
  };

  template<>
  struct context_factories::apply<ElementBase, svgpp::tag::element::use_>
  {
    typedef svgpp::factory::context::on_stack<Use> type;
  };

  template<class ElementTag>
  struct context_factories::apply<ElementBase, ElementTag>
  {
    typedef svgpp::factory::context::on_stack<Path> type;
  };

  length_factory_t length_factory_instance;

  struct length_policy_t
  {
    typedef length_factory_t const length_factory_type;

    static length_factory_type & length_factory(ElementBase const &)
    {
      return length_factory_instance;
    }
  };

  typedef svgpp::document_traversal<
#if defined(RENDERER_SKIA)
    svgpp::number_type<SkScalar>,
#elif defined(RENDERER_GDIPLUS)
    svgpp::number_type<Gdiplus::REAL>,
#endif
    svgpp::context_factories<context_factories>,
		svgpp::processed_elements<processed_elements>,
		svgpp::processed_attributes<processed_attributes>,
    svgpp::path_policy<path_policy>,
    svgpp::transform_events_policy<svgpp::policy::transform_events::forward_to_method<ElementBase> >,
    svgpp::length_policy<length_policy_t>
  > document_traversal;

  void Use::on_exit_element()
  {
    if (!display_)
      return;
    if (XMLElement element = xml_document_.findElementById(fragment_id_))
    {
#if defined(RENDERER_AGG)
      transform_.premultiply(agg::trans_affine_translation(x_, y_));
#elif defined(RENDERER_GDIPLUS)
      transform_.Translate(x_, y_);
#endif
#if !defined(RENDERER_SKIA) 
      // TODO:
      document_traversal::load_referenced_element<
        svgpp::referencing_element<svgpp::tag::element::use_>,
        svgpp::expected_elements<svgpp::traits::shape_elements>
      >::load(element, static_cast<ElementBase &>(*this));
#endif
    }
    else
      std::cerr << "Element referenced by 'use' not found\n";
  }
}

void ClipBuffer::intersectClipPath(XMLDocument & xml_document, svg_string_t const & id, transform_t const & transform)
{
  if (XMLElement node = xml_document.findElementById(id))
  {
    try
    {
#if defined(RENDERER_AGG)
      std::vector<unsigned char> clip_path_buffer(width_ * height_, 0xff);
      agg::rendering_buffer clip_path_rbuf(&clip_path_buffer[0], width_, height_, width_);
      ElementBase root_context(xml_document, clip_path_rbuf, transform);
      document_traversal::load_expected_element(node, root_context, svgpp::tag::element::clipPath());

      typedef agg::amask_no_clip_gray8 alpha_mask_t;
      alpha_mask_t clip_path_alpha_mask(clip_path_rbuf);
      agg::rendering_buffer rbuf(&buffer_[0], width_, height_, width_);
      pixfmt_t buffer_pixfmt(rbuf);
      typedef agg::pixfmt_amask_adaptor<pixfmt_t, alpha_mask_t> pixfmt_masked_t;
      pixfmt_masked_t pixfmt_masked(buffer_pixfmt, clip_path_alpha_mask);
      agg::renderer_base<pixfmt_masked_t> renderer_base(pixfmt_masked);
      renderer_base.clear(agg::gray8(0));
#endif
    } 
    catch (std::exception const & e)
    {
      std::cerr << "Error loading clipPath \"" << std::string(id.begin(), id.end()) << "\": " << e.what() << "\n";
    }
  }
}

