#pragma once

#include <svgpp/definitions.hpp>
#include <algorithm>

namespace svgpp
{

template<class Coordinate>
struct calculate_viewbox_transform
{
  template<class MeetOrSliceTag>
  static void calculate(
    Coordinate viewport_x, Coordinate viewport_y, Coordinate viewport_width, Coordinate viewport_height, 
    Coordinate viewbox_x, Coordinate viewbox_y, Coordinate viewbox_width, Coordinate viewbox_height,
    tag::value::none,
    MeetOrSliceTag, // ignored
    Coordinate & translate_x, Coordinate & translate_y, Coordinate & scale_x, Coordinate & scale_y)
  {
    scale_x = viewport_width / viewbox_width;
    scale_y = viewport_height / viewbox_height;
    translate_x = viewport_x - viewbox_x * scale_x;
    translate_y = viewport_y - viewbox_y * scale_y;
  }

  template<class AlignTag, class MeetOrSliceTag>
  static void calculate(
    Coordinate viewport_x, Coordinate viewport_y, Coordinate viewport_width, Coordinate viewport_height, 
    Coordinate viewbox_x, Coordinate viewbox_y, Coordinate viewbox_width, Coordinate viewbox_height,
    AlignTag,
    MeetOrSliceTag meetOrSlice_tag,
    Coordinate & translate_x, Coordinate & translate_y, Coordinate & scale_x, Coordinate & scale_y)
  {
    // TODO: check for 0
    scale_x = choose_scale(meetOrSlice_tag, viewport_width / viewbox_width, viewport_height / viewbox_height);
    scale_y = scale_x;
    translate_x = get_translate(typename AlignTag::x(), viewport_x, viewport_width, viewbox_x, viewbox_width, scale_x);
    translate_y = get_translate(typename AlignTag::y(), viewport_y, viewport_height, viewbox_y, viewbox_height, scale_y);
  }

private:
  static Coordinate choose_scale(tag::value::meet, Coordinate scale_x, Coordinate scale_y)
  {
    return std::min(scale_x, scale_y);
  }

  static Coordinate choose_scale(tag::value::slice, Coordinate scale_x, Coordinate scale_y)
  {
    return std::max(scale_x, scale_y);
  }

  static Coordinate get_translate(
    tag::min_tag,
    Coordinate viewport_offset, Coordinate viewport_size, 
    Coordinate viewbox_offset, Coordinate viewbox_size,
    Coordinate scale)
  { 
    return viewport_offset - scale * viewbox_offset;
  }

  static Coordinate get_translate(
    tag::mid_tag,
    Coordinate viewport_offset, Coordinate viewport_size, 
    Coordinate viewbox_offset, Coordinate viewbox_size,
    Coordinate scale)
  { 
    return viewport_offset + viewport_size/2 - scale * (viewbox_offset + viewbox_size/2);
  }

  static Coordinate get_translate(
    tag::max_tag,
    Coordinate viewport_offset, Coordinate viewport_size, 
    Coordinate viewbox_offset, Coordinate viewbox_size,
    Coordinate scale)
  { 
    return viewport_offset + viewport_size - scale * (viewbox_offset + viewbox_size);
  }
};

}
