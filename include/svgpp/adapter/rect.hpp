#pragma once

#include <svgpp/adapter/path.hpp>
#include <boost/mpl/set.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

namespace svgpp
{

typedef boost::mpl::set6<
  tag::attribute::x, 
  tag::attribute::y, 
  tag::attribute::rx, 
  tag::attribute::ry,
  tag::attribute::width, 
  tag::attribute::height
> rect_shape_attributes;

// TODO: add option to collect rect attributes without length conversion to coordinates

template<class Length>
class collect_rect_attributes_adapter: boost::noncopyable
{
public:
  template<class Context>
  bool on_exit_attributes(Context & context) const
  {
    typedef typename detail::unwrap_context<Context, tag::length_policy> length_policy_context;
    typedef typename length_policy_context::policy length_policy_t;
    typedef typename detail::unwrap_context<Context, tag::error_policy> error_policy;

    typename length_policy_t::length_factory_type & converter 
      = length_policy_t::length_factory(length_policy_context::get(context));

    typename length_policy_t::length_factory_type::coordinate_type
      x = 0, y = 0, rx = 0, ry = 0, width, height;
    if (x_)
      x = converter.length_to_user_coordinate(*x_, tag::width_length());
    if (y_)
      y = converter.length_to_user_coordinate(*y_, tag::height_length());
    width = converter.length_to_user_coordinate(width_, tag::width_length());
    height = converter.length_to_user_coordinate(height_, tag::height_length());
    if (rx_)
    {
      rx = converter.length_to_user_coordinate(*rx_, tag::width_length());
      if (!ry_)
        ry = rx;
    }
    if (ry_)
    {
      ry = converter.length_to_user_coordinate(*ry_, tag::height_length());
      if (!rx_)
        rx = ry;
    }
    if (rx < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::rx());
    if (ry < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::ry());
    if (width < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::width());
    if (height < 0)
      return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::height());

    if (width == 0 || height == 0)
      return true;

    typedef typename detail::unwrap_context<Context, tag::load_value_policy> load_value;
    load_value::policy::set_rect(load_value::get(context), x, y, width, height, rx, ry);
    return true;
  }

  void set(tag::attribute::x, Length const & val) { x_ = val; }
  void set(tag::attribute::y, Length const & val) { y_ = val; }
  void set(tag::attribute::rx, Length const & val) { rx_ = val; }
  void set(tag::attribute::ry, Length const & val) { ry_ = val; }
  void set(tag::attribute::width, Length const & val) { width_ = val; }
  void set(tag::attribute::height, Length const & val) { height_ = val; }

private:
  boost::optional<Length> x_, y_, rx_, ry_;
  Length width_, height_; // Required attributes
};

namespace detail
{
  template<class LoadPolicy, class Context, class Coordinate>
  inline void context_set_rounded_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height,
    Coordinate rx, Coordinate ry)
  {
    rx = std::min(rx, width / 2);
    ry = std::min(ry, height / 2);
    LoadPolicy::path_move_to(context, x + rx, y, tag::absolute_coordinate());
    LoadPolicy::path_line_to_ortho(context, x+width-rx, true, tag::absolute_coordinate());
    LoadPolicy::path_elliptical_arc_to(context, rx, ry, Coordinate(0), false, true, x+width, y+ry, tag::absolute_coordinate()); 
    LoadPolicy::path_line_to_ortho(context, y+height-ry, false, tag::absolute_coordinate());
    LoadPolicy::path_elliptical_arc_to(context, rx, ry, Coordinate(0), false, true, x+width-rx, y+height, tag::absolute_coordinate()); 
    LoadPolicy::path_line_to_ortho(context, x+rx, true, tag::absolute_coordinate());
    LoadPolicy::path_elliptical_arc_to(context, rx, ry, Coordinate(0), false, true, x, y+height-ry, tag::absolute_coordinate()); 
    LoadPolicy::path_line_to_ortho(context, y+ry, false, tag::absolute_coordinate());
    LoadPolicy::path_elliptical_arc_to(context, rx, ry, Coordinate(0), false, true, x+rx, y, tag::absolute_coordinate()); 
    LoadPolicy::path_close_subpath(context);
    LoadPolicy::path_exit(context);
  }
}

struct rect_to_path_adapter
{
  template<class Context, class Coordinate>
  static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height,
    Coordinate rx, Coordinate ry)
  {
    typedef typename detail::unwrap_context<Context, tag::load_path_policy> load_path;

    load_path::type & path_context = load_path::get(context);
    if (rx == 0 || ry == 0)
    {
      load_path::policy::path_move_to(path_context, x, y, tag::absolute_coordinate());
      load_path::policy::path_line_to_ortho(path_context, width,  true,  tag::relative_coordinate());
      load_path::policy::path_line_to_ortho(path_context, height, false, tag::relative_coordinate());
      load_path::policy::path_line_to_ortho(path_context, -width, true,  tag::relative_coordinate());
      load_path::policy::path_close_subpath(path_context);
      load_path::policy::path_exit(path_context);
    }
    else
    {
      detail::context_set_rounded_rect<load_path::policy>(path_context, x, y, width, height, rx, ry);
    }
  }
};

struct rounded_rect_to_path_adapter
{
  template<class Context, class Coordinate>
  static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height,
    Coordinate rx, Coordinate ry)
  {
    if (rx == 0 || ry == 0)
    {
      typedef typename detail::unwrap_context<Context, tag::load_value_policy> load_value;
      load_value::policy::set_rect(load_value::get(context), x, y, width, height);
    }
    else
    {
      typedef typename detail::unwrap_context<Context, tag::load_path_policy> load_path;
      detail::context_set_rounded_rect<load_path::policy>(load_path::get(context), x, y, width, height, rx, ry);
    }
  }
};

}