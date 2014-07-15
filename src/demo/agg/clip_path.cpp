#include "clip_path.hpp"

#include <agg_path_storage.h>
#include <agg_pixfmt_gray.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_renderer_base.h>
#include <agg_renderer_scanline.h>
#include <agg_scanline_p.h>

typedef agg::pixfmt_gray8 pixfmt_t;
typedef agg::renderer_base<pixfmt_t> renderer_base_t;

ClipBuffer::ClipBuffer(int width, int height)
  : buffer_(width * height)
  , rbuf_(&buffer_[0], width, height, width)
  , alpha_mask_(rbuf_)
{
  pixfmt_t pixfmt(rbuf_);
  renderer_base_t renderer_base(pixfmt);
  renderer_base.clear(pixfmt_t::color_type(0xFF));
}

ClipBuffer::ClipBuffer(ClipBuffer const & src)
  : buffer_(src.buffer_)
  , rbuf_(&buffer_[0], src.rbuf_.width(), src.rbuf_.height(), src.rbuf_.width())
  , alpha_mask_(rbuf_)
{}

void ClipBuffer::intersect_clip_rect(agg::trans_affine const & transform, double x, double y, double width, double height)
{
  typedef agg::renderer_scanline_aa_solid<renderer_base_t> renderer_t;

  pixfmt_t pixfmt(rbuf_);
  renderer_base_t renderer_base(pixfmt);
  agg::scanline_p8 scanline;

  /*agg::path_storage path_storage;
  path_storage.move_to(x, y);
  path_storage.line_to(x + width, y);
  path_storage.line_to(x + width, y + height);
  path_storage.line_to(x, y + height);
  path_storage.close_polygon();

  agg::conv_transform<agg::path_storage> transformed(path_storage, transform);*/

  agg::rasterizer_scanline_aa<> rasterizer;
  rasterizer.filling_rule(agg::fill_even_odd);
  //rasterizer.add_path(transformed);

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

#if 0
boost::optional<ClipPath> ClipPaths::get(
    svg_string_t const & id, 
    length_factory_t const & length_factory/*, 
    get_bounding_box_func_t const & get_bounding_box*/)
{
  // TODO: inheritance via xlink::href
  if (XMLElement node = xml_document_.find_element_by_id(id))
  {
    try
    {
      GradientContext gradient_context(length_factory);
      svgpp::document_traversal<
        svgpp::context_factories<gradient_context_factories>,
        svgpp::attribute_traversal_policy<attribute_traversal>,
        svgpp::processed_elements<
          boost::mpl::set<
            svgpp::tag::element::linearGradient,
            svgpp::tag::element::radialGradient,
            svgpp::tag::element::stop
          > 
        >,
        svgpp::processed_attributes<
          boost::mpl::set<
            svgpp::tag::attribute::x1,
            svgpp::tag::attribute::y1,
            svgpp::tag::attribute::x2,
            svgpp::tag::attribute::y2,
            svgpp::tag::attribute::fx,
            svgpp::tag::attribute::fy,
            svgpp::tag::attribute::cx,
            svgpp::tag::attribute::cy,
            svgpp::tag::attribute::r,
            svgpp::tag::attribute::gradientUnits,
            svgpp::tag::attribute::gradientTransform,
            svgpp::tag::attribute::spreadMethod,
            svgpp::tag::attribute::offset,
            boost::mpl::pair<svgpp::tag::element::stop, svgpp::tag::attribute::stop_color>,
            boost::mpl::pair<svgpp::tag::element::stop, svgpp::tag::attribute::stop_opacity>
          >
        >
      >::load_referenced_element<
        void, // Doesn't matter
        svgpp::traits::gradient_elements, 
        svgpp::processed_elements<svgpp::traits::gradient_elements>
      >(node, gradient_context);
      return gradient_context.gradient_;
    } catch (std::exception const & e)
    {
      std::cerr << "Error loading paint \"" << std::string(id.begin(), id.end()) << "\": " << e.what() << "\n";
    }
  }
  return boost::optional<Gradient>();
}

#endif