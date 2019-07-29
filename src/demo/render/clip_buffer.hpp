#pragma once

#include <vector>
#include <boost/gil/typedefs.hpp>
#include "common.hpp"

class ClipBuffer
{
public:
  ClipBuffer(int width, int height);
  ClipBuffer(ClipBuffer const & src);

  boost::gil::gray8c_view_t gilView() const;

  void intersectClipRect(transform_t const & transform, number_t x, number_t y, number_t width, number_t height);
  void intersectClipPath(XMLDocument & xml_document, svg_string_t const & id, transform_t const & transform);

private:
  std::vector<unsigned char> buffer_;
  const int width_, height_;
};