#pragma once

#include <agg_alpha_mask_u8.h>
#include <agg_trans_affine.h>
#include <vector>

class ClipBuffer
{
public:
  typedef agg::amask_no_clip_gray8 alpha_mask_t;

  ClipBuffer(int width, int height);
  ClipBuffer(ClipBuffer const & src);

  alpha_mask_t const & alphaMask() const { return alpha_mask_; }

  void intersectClipRect(agg::trans_affine const & transform, double x, double y, double width, double height);

private:
  std::vector<unsigned char> buffer_;
  agg::rendering_buffer rbuf_;
  alpha_mask_t alpha_mask_;
};