#include "clip_buffer.hpp"

#include <agg_conv_curve.h>
#include <agg_path_storage.h>
#include <agg_pixfmt_amask_adaptor.h>
#include <agg_pixfmt_gray.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_renderer_base.h>
#include <agg_renderer_scanline.h>
#include <agg_scanline_p.h>
#include <svgpp/svgpp.hpp>

typedef agg::pixfmt_gray8 pixfmt_t;
typedef agg::renderer_base<pixfmt_t> renderer_base_t;

ClipBuffer::ClipBuffer(int width, int height)
  : buffer_(width * height, 0xff)
  , rbuf_(&buffer_[0], width, height, width)
{}

ClipBuffer::ClipBuffer(ClipBuffer const & src)
  : buffer_(src.buffer_)
  , rbuf_(&buffer_[0], src.rbuf_.width(), src.rbuf_.height(), src.rbuf_.width())
{}

void ClipBuffer::intersectClipRect(agg::trans_affine const & transform, double x, double y, double width, double height)
{
  typedef agg::renderer_scanline_aa_solid<renderer_base_t> renderer_t;

  pixfmt_t pixfmt(rbuf_);
  renderer_base_t renderer_base(pixfmt);
  agg::scanline_p8 scanline;

  agg::rasterizer_scanline_aa<> rasterizer;
  rasterizer.filling_rule(agg::fill_even_odd);

  rasterizer.move_to_d(0, 0);
  rasterizer.line_to_d(pixfmt.width(), 0);
  rasterizer.line_to_d(pixfmt.width(), pixfmt.height());
  rasterizer.line_to_d(0, pixfmt.height());
  rasterizer.close_polygon();

  double px = x, py = y;
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
}

namespace 
{
  typedef boost::mpl::insert<
    svgpp::traits::shape_elements,
    svgpp::tag::element::use_
  >::type processed_elements;

  struct path_policy: svgpp::policy::path::no_shorthands
  {
    static const bool arc_as_cubic_bezier = true; 
  };

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
      agg::rendering_buffer & rbuf, 
      agg::trans_affine const & transform
      )
      : xml_document_(xml_document)
      , rbuf_(rbuf)
      , transform_(transform)
      , display_(true)
      , nonzero_clip_rule_(true)
    {}

    ElementBase(ElementBase const & parent)
      : xml_document_(parent.xml_document_)
      , rbuf_(parent.rbuf_)
      , transform_(parent.transform_)
      , display_(parent.display_)
      , nonzero_clip_rule_(parent.nonzero_clip_rule_)
    {}

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

    void set_transform_matrix(const boost::array<double, 6> & matrix)
    {
      transform_.premultiply(agg::trans_affine(matrix.data()));
    }

  protected:
    XMLDocument & xml_document_;
    agg::rendering_buffer & rbuf_;
    agg::trans_affine transform_;
    bool display_;
    bool nonzero_clip_rule_;
  };

  class Path: public ElementBase
  {
  public:
    Path(ElementBase const & parent)
      : ElementBase(parent)
    {}

    void path_move_to(double x, double y, svgpp::tag::coordinate::absolute const &)
    { 
      path_storage_.move_to(x, y);
    }

    void path_line_to(double x, double y, svgpp::tag::coordinate::absolute const &)
    { 
      path_storage_.line_to(x, y);
    }

    void path_cubic_bezier_to(
      double x1, double y1, 
      double x2, double y2, 
      double x, double y, 
      svgpp::tag::coordinate::absolute const &)
    { 
      path_storage_.curve4(x1, y1, x2, y2, x, y);
    }

    void path_quadratic_bezier_to(
      double x1, double y1, 
      double x, double y, 
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

    void on_exit_element()
    {
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
    }

  private:
    agg::path_storage path_storage_;
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

    void set(svgpp::tag::attribute::x, double val)
    { x_ = val; }

    void set(svgpp::tag::attribute::y, double val)
    { y_ = val; }

  private:
    svg_string_t fragment_id_;
    double x_, y_;
  };

  struct context_factories
  {
    template<class ParentContext, class ElementTag>
    struct apply;
  };

  template<>
  struct context_factories::apply<ElementBase, svgpp::tag::element::use_>
  {
    typedef svgpp::factory::context::on_stack<ElementBase, Use> type;
  };

  template<class ElementTag>
  struct context_factories::apply<ElementBase, ElementTag>
  {
    typedef svgpp::factory::context::on_stack<ElementBase, Path> type;
  };

  typedef svgpp::document_traversal<
    svgpp::context_factories<context_factories>,
		svgpp::processed_elements<processed_elements>,
		svgpp::processed_attributes<processed_attributes>,
    svgpp::path_policy<path_policy>
  > document_traversal;

  void Use::on_exit_element()
  {
    if (!display_)
      return;
    if (XMLElement element = xml_document_.findElementById(fragment_id_))
    {
      transform_.premultiply(agg::trans_affine_translation(x_, y_));
      document_traversal::load_referenced_element<
        svgpp::referencing_element<svgpp::tag::element::use_>,
        svgpp::expected_elements<svgpp::traits::shape_elements>
      >::load(element, static_cast<ElementBase &>(*this));
    }
    else
      std::cerr << "Element referenced by 'use' not found\n";
  }
}

void ClipBuffer::intersectClipPath(XMLDocument & xml_document, svg_string_t const & id, agg::trans_affine const & transform)
{
  if (XMLElement node = xml_document.findElementById(id))
  {
    try
    {
      std::vector<unsigned char> clip_path_buffer(rbuf_.width() * rbuf_.height(), 0xff);
      agg::rendering_buffer clip_path_rbuf(&clip_path_buffer[0], rbuf_.width(), rbuf_.height(), rbuf_.width());
      ElementBase root_context(xml_document, clip_path_rbuf, transform);
      document_traversal::load_expected_element<void>(node, root_context, svgpp::tag::element::clipPath());

      typedef agg::amask_no_clip_gray8 alpha_mask_t;
      alpha_mask_t clip_path_alpha_mask(clip_path_rbuf);
      pixfmt_t buffer_pixfmt(rbuf_);
      typedef agg::pixfmt_amask_adaptor<pixfmt_t, alpha_mask_t> pixfmt_masked_t;
      pixfmt_masked_t pixfmt_masked(buffer_pixfmt, clip_path_alpha_mask);
      agg::renderer_base<pixfmt_masked_t> renderer_base(pixfmt_masked);
      renderer_base.clear(agg::gray8(0));
    } 
    catch (std::exception const & e)
    {
      std::cerr << "Error loading clipPath \"" << std::string(id.begin(), id.end()) << "\": " << e.what() << "\n";
    }
  }
}

