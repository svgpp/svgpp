#pragma once

#include <svgpp/definitions.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/mpl/set.hpp>

namespace svgpp
{

typedef boost::mpl::set4<tag::attribute::cx, tag::attribute::cy, tag::attribute::rx, tag::attribute::ry>
  ellipse_shape_attributes;

template<class Length>
class collect_ellipse_attributes_adapter: boost::noncopyable
{
public:
  template<class Context>
  bool on_exit_attributes(Context & context) const
  {
    typedef typename detail::unwrap_context<Context, tag::error_policy> error_policy;
    typedef typename detail::unwrap_context<Context, tag::length_policy> length_policy_context;
    typedef typename length_policy_context::policy length_policy_t;

    typename length_policy_t::length_factory_type & converter 
      = length_policy_t::length_factory(length_policy_context::get(context));

    typename length_policy_t::length_factory_type::coordinate_type
      cx = 0, cy = 0, rx = 0, ry = 0;
    if (cx_)
      cx = converter.length_to_user_coordinate(*cx_, tag::width_length());
    if (cy_)
      cy = converter.length_to_user_coordinate(*cy_, tag::height_length());
    if (rx_ && ry_) // required attributes
    {
      rx = converter.length_to_user_coordinate(*rx_, tag::width_length());
      if (rx < 0)
        return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::rx());
      if (rx == 0)
        return true;
      ry = converter.length_to_user_coordinate(*ry_, tag::height_length());
      if (ry < 0)
        return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::ry());
      if (ry == 0)
        return true;
      typedef typename detail::unwrap_context<Context, tag::load_value_policy> load_value;
      load_value::policy::set_ellipse(load_value::get(context), cx, cy, rx, ry);
    }
    return true;
  }

  void set(tag::attribute::cx, Length const & val) { cx_ = val; }
  void set(tag::attribute::cy, Length const & val) { cy_ = val; }
  void set(tag::attribute::rx, Length const & val) { rx_ = val; }
  void set(tag::attribute::ry, Length const & val) { ry_ = val; }

private:
  boost::optional<Length> cx_, cy_, rx_, ry_;
};

struct ellipse_to_path_adapter
{
  template<class Context, class Coordinate>
  static void set_ellipse(Context & context, Coordinate cx, Coordinate cy, Coordinate rx, Coordinate ry)
  {
    typedef typename detail::unwrap_context<Context, tag::load_path_policy> load_path;

    load_path::type & path_context = load_path::get(context);
    load_path::policy::path_move_to(path_context, cx + rx, cy, tag::absolute_coordinate());
    load_path::policy::path_elliptical_arc_to(path_context, rx, ry, 0, false, true, cx - rx, cy, tag::absolute_coordinate());
    load_path::policy::path_elliptical_arc_to(path_context, rx, ry, 0, false, true, cx + rx, cy, tag::absolute_coordinate());
    load_path::policy::path_close_subpath(path_context);
    load_path::policy::path_exit(path_context);
  }
};

}