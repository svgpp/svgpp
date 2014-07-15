#pragma once

#include <svgpp/utility/calculate_viewbox_transform.hpp>
#include <boost/optional.hpp>
#include <boost/mpl/set.hpp>
#include <boost/noncopyable.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/variant.hpp>

namespace svgpp
{

// Pairs of <referencing element tag, referenced element tag> combinations in which 
// calculate_viewport_adapter needs to know type of referenced element
typedef boost::mpl::set3<
  boost::mpl::pair<tag::element::use_,  tag::element::svg>,
  boost::mpl::pair<tag::element::image, tag::element::svg>,
  boost::mpl::pair<tag::element::use_,  tag::element::symbol>
> viewport_adapter_needs_to_know_referencing_element;

namespace tag { namespace viewport_size_source
{
  struct use_own; // 'width' and 'height' attributes are set on element
  struct use_reference; // Like 'symbol' element referenced by 'use'
  struct reference_override_own; // Like 'svg' element referenced by 'use'
}}

template<
  class ReferencingElementTag, // 'void' means that element is not referenced or
                               // referencing element type doesn't matter (e.g. 'symbol' may be referenced only by 'use')
                               // see viewport_adapter_needs_to_know_referencing_element
  class ElementTag
>
struct get_viewport_size_source; // Shouldn't be called for anything other than 'svg' and 'symbol',
                                 // referenced by 'use' or 'image'
                                 // 'symbol' must only be used while referenced by 'use'

template<>
struct get_viewport_size_source<tag::element::use_, tag::element::svg>
{
  typedef tag::viewport_size_source::reference_override_own type;
};

template<>
struct get_viewport_size_source<tag::element::image, tag::element::svg>
{
  typedef tag::viewport_size_source::use_reference type;
};

template<>
struct get_viewport_size_source<void, tag::element::svg>
{
  typedef tag::viewport_size_source::use_own type;
};

template<>
struct get_viewport_size_source<tag::element::use_, tag::element::symbol>
{
  typedef tag::viewport_size_source::use_reference type;
};

namespace detail
{
  template<class Length, class Coordinate, class ViewportSizeSourceTag>
  class calculate_viewport_adapter_size_holder;

  template<class Length, class Coordinate>
  class calculate_viewport_adapter_size_holder<Length, Coordinate, tag::viewport_size_source::use_own>
  {
  public:
    void set(tag::attribute::width,         Length const & val) { viewport_width_ = val; }
    void set(tag::attribute::markerWidth,   Length const & val) { viewport_width_ = val; }
    void set(tag::attribute::height,        Length const & val) { viewport_height_ = val; }
    void set(tag::attribute::markerHeight,  Length const & val) { viewport_height_ = val; }

  protected:
    template<class LoadPolicy, class Context, class LengthToUserCoordinatesConverter>
    void get_viewport_size(Context & context, LengthToUserCoordinatesConverter const & converter,
      Coordinate & viewport_width, Coordinate & viewport_height) const
    {
      viewport_width = converter.length_to_user_coordinate(
        viewport_width_
          ? *viewport_width_
          : converter.create_length(100, tag::length_units::percent(), tag::width_length()),
        tag::width_length());
      viewport_height = converter.length_to_user_coordinate(
        viewport_height_
          ? *viewport_height_
          : converter.create_length(100, tag::length_units::percent(), tag::height_length()),
        tag::height_length());
    }

  private:
    boost::optional<Length> viewport_width_, viewport_height_;
  };

  template<class Length, class Coordinate>
  class calculate_viewport_adapter_size_holder<Length, Coordinate, tag::viewport_size_source::use_reference>
  {
  protected:
    template<class LoadPolicy, class Context, class LengthToUserCoordinatesConverter>
    void get_viewport_size(Context & context, LengthToUserCoordinatesConverter const & converter,
      Coordinate & viewport_width, Coordinate & viewport_height) const
    {
      // Init with defaults first
      viewport_width = converter.length_to_user_coordinate(
        converter.create_length(100, tag::length_units::percent(), tag::width_length()),
        tag::width_length());
      viewport_height = converter.length_to_user_coordinate(
        converter.create_length(100, tag::length_units::percent(), tag::height_length()),
        tag::height_length());

      LoadPolicy::get_reference_viewport_size(context, viewport_width, viewport_height);
    }

    struct some_dummy_type;
    void set(some_dummy_type); // The only purpose of this declaration is to make
                               // "use base_type::set;" possible in inherited type
  };

  template<class Length, class Coordinate>
  class calculate_viewport_adapter_size_holder<Length, Coordinate, tag::viewport_size_source::reference_override_own>
    : public calculate_viewport_adapter_size_holder<Length, Coordinate, tag::viewport_size_source::use_own>
  {
  protected:
    template<class LoadPolicy, class Context, class LengthToUserCoordinatesConverter>
    void get_viewport_size(Context & context, LengthToUserCoordinatesConverter const & converter,
      Coordinate & viewport_width, Coordinate & viewport_height) const
    {
      calculate_viewport_adapter_size_holder<Length, Coordinate, tag::viewport_size_source::use_own>
        ::get_viewport_size<LoadPolicy>(context, converter, viewport_width, viewport_height);
      // get_reference_viewport_size must only set values which was specified for referenced element
      // and keep values from current element if referenced element lacks them
      LoadPolicy::get_reference_viewport_size(context, viewport_width, viewport_height);
    }
  };
}

template<class Length, class Coordinate, class ViewportSizeSourceTag>
class calculate_viewport_adapter: 
  public detail::calculate_viewport_adapter_size_holder<Length, Coordinate, ViewportSizeSourceTag>,
  boost::noncopyable
{
  typedef detail::calculate_viewport_adapter_size_holder<Length, Coordinate, ViewportSizeSourceTag> base_type;
  typedef boost::tuple<Coordinate, Coordinate, Coordinate, Coordinate> viewbox_type; // x, y, width, height

public:
  calculate_viewport_adapter()
    : defer_(false)
    , align_(tag::value::xMidYMid())
    , meetOrSlice_(tag::value::meet())
  {}

  template<class Context>
  bool on_exit_attributes(Context & context) const
  {
    typedef typename detail::unwrap_context<Context, tag::load_value_policy> load_value;
    typedef typename detail::unwrap_context<Context, tag::error_policy> error_policy;
    typedef typename detail::unwrap_context<Context, tag::length_policy> length_policy_context;
    typedef typename length_policy_context::policy length_policy_t;

    typename length_policy_t::length_factory_type & converter 
      = length_policy_t::length_factory(length_policy_context::get(context));

    Coordinate viewport_x = converter.length_to_user_coordinate(
      viewport_x_ ? *viewport_x_ : converter.create_length(0, tag::length_units::none()),
      tag::width_length());
    Coordinate viewport_y = converter.length_to_user_coordinate(
      viewport_y_ ? *viewport_y_ : converter.create_length(0, tag::length_units::none()),
      tag::height_length());

    Coordinate viewport_width = 0, viewport_height = 0;
    base_type::get_viewport_size<load_value::policy>(context, converter, viewport_width, viewport_height);

    if (viewport_width == 0 || viewport_height == 0)
      // TODO: disable rendering
      return true;

    if (viewport_width < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::width());
    if (viewport_height < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::height());

    load_value::policy::set_viewport(load_value::get(context), viewport_x, viewport_y, viewport_width, viewport_height);

    if (viewbox_)
    {
      if (viewbox_->get<2>() == 0 || viewbox_->get<3>() == 0)
        // TODO: disable rendering
        return true;
      if (viewbox_->get<2>() < 0 || viewbox_->get<3>() < 0)
        return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::viewBox());

      Coordinate translate_x, translate_y, scale_x, scale_y;
      boost::apply_visitor(
        options_visitor<length_policy_t::length_factory_type>(*viewbox_,
          viewport_x, viewport_y, viewport_width, viewport_height,
          translate_x, translate_y, scale_x, scale_y), 
        align_, meetOrSlice_);
      load_value::policy::set_viewport_transform(load_value::get(context), translate_x, translate_y, scale_x, scale_y, defer_);
    }
    else
      load_value::policy::set_viewport_transform(load_value::get(context), viewport_x, viewport_y);
    return true;
  }

  using base_type::set;
  void set(tag::attribute::x,             Length const & val) { viewport_x_ = val; }
  void set(tag::attribute::y,             Length const & val) { viewport_y_ = val; }
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
  boost::optional<Length> viewport_x_, viewport_y_;
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
      calculate_viewbox_transform<Coordinate>::calculate(
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

struct viewport_transform_adapter
{
  template<class Context, class Coordinate>
  static void set_viewport_transform(Context & context, Coordinate translate_x, Coordinate translate_y, 
    Coordinate scale_x, Coordinate scale_y, bool)
  {
    typedef typename detail::unwrap_context<Context, tag::load_transform_policy> load_transform;

    load_transform::policy::append_transform_translate(load_transform::get(context), translate_x, translate_y);
    load_transform::policy::append_transform_scale(load_transform::get(context), scale_x, scale_y);
  }

  template<class Context, class Coordinate>
  static void set_viewport_transform(Context & context, Coordinate translate_x, Coordinate translate_y)
  {
    typedef typename detail::unwrap_context<Context, tag::load_transform_policy> load_transform;

    load_transform::policy::append_transform_translate(load_transform::get(context), translate_x, translate_y);
  }

  template<class Context, class Coordinate>
  static void set_viewport(Context & context, Coordinate viewport_x, Coordinate viewport_y, 
    Coordinate viewport_width, Coordinate viewport_height)
  {
    typedef typename detail::unwrap_context<Context, tag::load_value_policy> load_value;

    load_value::policy::set_viewport(load_value::get(context), viewport_x, viewport_y, viewport_width, viewport_height);
  }

  template<class Context, class Coordinate>
  static void get_reference_viewport_size(Context & context, Coordinate & viewport_width, Coordinate & viewport_height) 
  {
    typedef typename detail::unwrap_context<Context, tag::load_value_policy> load_value;

    load_value::policy::get_reference_viewport_size(load_value::get(context), viewport_width, viewport_height);
  }
};

}