#pragma once

#include <svgpp/utility/calculate_viewport_transform.hpp>
#include <svgpp/context_policy_load_transform.hpp>
#include <svgpp/context_policy_load_value.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/variant.hpp>

namespace svgpp
{

template<class Length, class Coordinate, class LoadPolicy = void>
class calculate_viewport_adapter: boost::noncopyable
{
  typedef boost::tuple<Coordinate, Coordinate, Coordinate, Coordinate> viewbox_type; // x, y, width, height

public:
  calculate_viewport_adapter()
    : defer_(false)
    , align_(tag::value::xMidYMid())
    , meetOrSlice_(tag::value::meet())
  {}

  template<class Context, class LengthToUserCoordinatesConverter>
  bool on_exit_attributes(Context & context, LengthToUserCoordinatesConverter const & converter) const
  {
    return on_exit_attributesT<
      context_policy<tag::error_policy, Context>
    >(context, converter);
  }

  template<class ErrorPolicy, class Context, class LengthToUserCoordinatesConverter>
  bool on_exit_attributesT(Context & context, LengthToUserCoordinatesConverter const & converter) const
  {
    typedef typename boost::mpl::if_<
      boost::is_same<LoadPolicy, void>,
      context_policy<tag::load_value_policy, Context>,
      LoadPolicy>::type load_policy;

    Coordinate viewport_x = converter.length_to_user_coordinate(
      viewport_x_ ? *viewport_x_ : converter.create_length(0, tag::length_units::none()),
      tag::width_length());
    Coordinate viewport_y = converter.length_to_user_coordinate(
      viewport_y_ ? *viewport_y_ : converter.create_length(0, tag::length_units::none()),
      tag::height_length());
    Coordinate viewport_width = converter.length_to_user_coordinate(
      viewport_width_
        ? *viewport_width_
        : converter.create_length(100, tag::length_units::percent(), tag::width_length()),
      tag::width_length());
    Coordinate viewport_height = converter.length_to_user_coordinate(
      viewport_height_
        ? *viewport_height_
        : converter.create_length(100, tag::length_units::percent(), tag::height_length()),
      tag::height_length());

    if (viewport_width == 0 || viewport_height == 0)
      // TODO: disable rendering
      return true;

    if (viewport_width < 0)
      return ErrorPolicy::negative_value(context, tag::attribute::width());
    if (viewport_height < 0)
      return ErrorPolicy::negative_value(context, tag::attribute::height());

    if (viewbox_)
    {
      if (viewbox_->get<2>() == 0 || viewbox_->get<3>() == 0)
        // TODO: disable rendering
        return true;
      if (viewbox_->get<2>() < 0 || viewbox_->get<3>() < 0)
        return ErrorPolicy::negative_value(context, tag::attribute::viewBox());

      Coordinate translate_x, translate_y, scale_x, scale_y;
      boost::apply_visitor(
        options_visitor<LengthToUserCoordinatesConverter>(*viewbox_,
          viewport_x, viewport_y, viewport_width, viewport_height,
          translate_x, translate_y, scale_x, scale_y), 
        align_, meetOrSlice_);
      // TODO: Pass also new viewport size to use it in percentage lengths
      load_policy::set_viewport_transform(context, translate_x, translate_y, scale_x, scale_y, defer_);
    }
    else
      load_policy::set_viewport_transform(context, viewport_x, viewport_y);
    return true;
  }

  void set(tag::attribute::x,      Length const & val) { viewport_x_ = val; }
  void set(tag::attribute::y,      Length const & val) { viewport_y_ = val; }
  void set(tag::attribute::width,  Length const & val) { viewport_width_ = val; }
  void set(tag::attribute::height, Length const & val) { viewport_height_ = val; }
  void set(tag::attribute::viewBox, Coordinate x, Coordinate y, Coordinate w, Coordinate h) 
  {
    viewbox_ = viewbox_type(x, y, w, h);
  }

  void set(tag::attribute::preserveAspectRatio, bool defer, tag::value::none align_tag)
  {
    defer_ = defer;
    align_ = align_tag;
  }

  template<class AlignTag, class MeetOrSliceTag>
  void set(tag::attribute::preserveAspectRatio, bool defer, AlignTag align_tag, MeetOrSliceTag meetOrSlice)
  {
    defer_ = defer;
    align_ = align_tag;
    meetOrSlice_ = meetOrSlice;
  }

private:
  boost::optional<Length> viewport_x_, viewport_y_, viewport_width_, viewport_height_;
  boost::optional<viewbox_type> viewbox_;
  bool defer_;
  boost::variant<
    tag::value::none, 
    tag::value::xMinYMin,
    tag::value::xMidYMin,
    tag::value::xMaxYMin,
    tag::value::xMinYMid,
    tag::value::xMidYMid,
    tag::value::xMaxYMid,
    tag::value::xMinYMax,
    tag::value::xMidYMax,
    tag::value::xMaxYMax> align_; 
  boost::variant<tag::value::meet, tag::value::slice> meetOrSlice_; 

  template<class LengthToUserCoordinatesConverter>
  class options_visitor: 
    public boost::static_visitor<>,
    boost::noncopyable
  {
  public:
    options_visitor(viewbox_type const & viewbox,
      Coordinate viewport_x, Coordinate viewport_y, Coordinate viewport_width, Coordinate viewport_height,
      Coordinate & translate_x, Coordinate & translate_y, Coordinate & scale_x, Coordinate & scale_y)
      : viewbox_(viewbox)
      , viewport_x_(viewport_x)
      , viewport_y_(viewport_y)
      , viewport_width_(viewport_width)
      , viewport_height_(viewport_height)
      , translate_x_(translate_x)
      , translate_y_(translate_y)
      , scale_x_(scale_x)
      , scale_y_(scale_y)
    {
    }

    template<class AlignTag, class MeetOrSliceTag>
    void operator()(const AlignTag & align_tag, const MeetOrSliceTag & meetOrSlice_tag) const
    {
      calculate_viewport_transform<Coordinate>::calculate(
        viewport_x_, viewport_y_, viewport_width_, viewport_height_,
        viewbox_.get<0>(), viewbox_.get<1>(), viewbox_.get<2>(), viewbox_.get<3>(),
        align_tag,
        meetOrSlice_tag,
        translate_x_, translate_y_, scale_x_, scale_y_);
    }

  private:
    viewbox_type const & viewbox_;
    Coordinate viewport_x_, viewport_y_, viewport_width_, viewport_height_;
    Coordinate & translate_x_, & translate_y_, & scale_x_, & scale_y_;
  };
};

template<class LoadPolicy = void>
struct viewport_transform_adapter
{
  template<class Context, class Coordinate>
  static void set_viewport_transform(Context & context, Coordinate translate_x, Coordinate translate_y, 
    Coordinate scale_x, Coordinate scale_y, bool)
  {
    typedef typename boost::mpl::if_<
      boost::is_same<LoadPolicy, void>,
      context_policy<tag::load_transform_policy, Context>,
      LoadPolicy>::type load_policy;

    load_policy::append_transform_translate(context, translate_x, translate_y);
    load_policy::append_transform_scale(context, scale_x, scale_y);
  }

  template<class Context, class Coordinate>
  static void set_viewport_transform(Context & context, Coordinate translate_x, Coordinate translate_y)
  {
    typedef typename boost::mpl::if_<
      boost::is_same<LoadPolicy, void>,
      context_policy<tag::load_transform_policy, Context>,
      LoadPolicy>::type load_policy;

    load_policy::append_transform_translate(context, translate_x, translate_y);
  }

private:
  viewport_transform_adapter();
};

}