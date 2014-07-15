#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/adapter/rect.hpp>
#include <svgpp/adapter/line.hpp>
#include <svgpp/adapter/circle.hpp>
#include <svgpp/adapter/ellipse.hpp>

namespace svgpp
{

template<class ElementTag>
struct basic_shape_attributes;

template<class ElementTag, class Length>
struct collect_basic_shape_attributes_adapter;

template<class ElementTag>
struct basic_shape_to_path_adapter;

template<>
struct basic_shape_attributes<tag::element::rect>
{
  typedef rect_shape_attributes type;
};

template<>
struct basic_shape_attributes<tag::element::circle>
{
  typedef circle_shape_attributes type;
};

template<>
struct basic_shape_attributes<tag::element::ellipse>
{
  typedef ellipse_shape_attributes type;
};

template<>
struct basic_shape_attributes<tag::element::line>
{
  typedef line_shape_attributes type;
};

template<class Length>
struct collect_basic_shape_attributes_adapter<tag::element::rect, Length>
{
  typedef collect_rect_attributes_adapter<Length> type;
};

template<class Length>
struct collect_basic_shape_attributes_adapter<tag::element::circle, Length>
{
  typedef collect_circle_attributes_adapter<Length> type;
};

template<class Length>
struct collect_basic_shape_attributes_adapter<tag::element::ellipse, Length>
{
  typedef collect_ellipse_attributes_adapter<Length> type;
};

template<class Length>
struct collect_basic_shape_attributes_adapter<tag::element::line, Length>
{
  typedef collect_line_attributes_adapter<Length> type;
};

template<>
struct basic_shape_to_path_adapter<tag::element::rect>
{
  typedef rect_to_path_adapter type;
};

template<>
struct basic_shape_to_path_adapter<tag::element::circle>
{
  typedef circle_to_path_adapter type;
};

template<>
struct basic_shape_to_path_adapter<tag::element::ellipse>
{
  typedef ellipse_to_path_adapter type;
};

template<>
struct basic_shape_to_path_adapter<tag::element::line>
{
  typedef line_to_path_adapter type;
};

}