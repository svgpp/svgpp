#pragma once

#include <agg_alpha_mask_u8.h>
#include <agg_trans_affine.h>
#include <vector>
#include <boost/gil/gil_all.hpp>
#include "common.hpp"

class ClipBuffer
{
public:
  ClipBuffer(int width, int height);
  ClipBuffer(ClipBuffer const & src);

  boost::gil::gray8c_view_t gilView() const
  {
    return boost::gil::interleaved_view(rbuf_.width(), rbuf_.height(), 
      reinterpret_cast<const boost::gil::gray8_pixel_t *>(&buffer_[0]), rbuf_.stride());
  }

  void intersectClipRect(agg::trans_affine const & transform, double x, double y, double width, double height);
  void intersectClipPath(XMLDocument & xml_document, svg_string_t const & id, agg::trans_affine const & transform);

private:
  std::vector<unsigned char> buffer_;
  agg::rendering_buffer rbuf_;
};