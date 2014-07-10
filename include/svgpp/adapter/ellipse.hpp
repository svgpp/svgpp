#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/policy/error.hpp>
#include <svgpp/policy/load_path.hpp>
#include <svgpp/policy/load_value.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/mpl/set.hpp>
#include <stdexcept>

namespace svgpp
{

typedef boost::mpl::set4<tag::attribute::cx, tag::attribute::cy, tag::attribute::rx, tag::attribute::ry>
  ellipse_shape_attributes;

template<class Length>
class collect_ellipse_attributes_adapter: boost::noncopyable
{
public:
  template<class Context, class LengthToUserCoordinatesConverter>
  bool on_exit_attributes(Context & context, LengthToUserCoordinatesConverter const & converter) const
  {
    return on_exit_attributesT<
      policy::error::default_policy<Context>,
      policy::load_value::default_policy<Context> 
    >(context, converter);
  }

  template<class ErrorPolicy, class LoadPolicy, class Context, class LengthToUserCoordinatesConverter>
  bool on_exit_attributesT(Context & context, LengthToUserCoordinatesConverter const & converter) const
  {
    typename LengthToUserCoordinatesConverter::coordinate_type
      cx = 0, cy = 0, rx = 0, ry = 0;
    if (cx_)
      cx = converter.length_to_user_coordinate(*cx_, tag::width_length());
    if (cy_)
      cy = converter.length_to_user_coordinate(*cy_, tag::height_length());
    if (rx_ && ry_) // required attributes
    {
      rx = converter.length_to_user_coordinate(*rx_, tag::width_length());
      if (rx < 0)
        return ErrorPolicy::negative_value(context, tag::attribute::rx());
      if (rx == 0)
        return true;
      ry = converter.length_to_user_coordinate(*ry_, tag::height_length());
      if (ry < 0)
        return ErrorPolicy::negative_value(context, tag::attribute::ry());
      if (ry == 0)
        return true;
      LoadPolicy::set_ellipse(context, cx, cy, rx, ry);
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

template<class LoadPathPolicy = void>
struct ellipse_to_path_adapter
{
  template<class Context, class Coordinate>
  static void set_ellipse(Context & context, Coordinate cx, Coordinate cy, Coordinate rx, Coordinate ry)
  {
    typedef typename boost::mpl::if_<
      boost::is_same<LoadPathPolicy, void>,
      policy::load_path::default_policy<Context>,
      LoadPathPolicy
    >::type load_policy;

    load_policy::path_move_to(context, cx + rx, cy, tag::absolute_coordinate());
    load_policy::path_elliptical_arc_to(context, rx, ry, 0, false, true, cx - rx, cy, tag::absolute_coordinate());
    load_policy::path_elliptical_arc_to(context, rx, ry, 0, false, true, cx + rx, cy, tag::absolute_coordinate());
    load_policy::path_close_subpath(context);
    load_policy::path_exit(context);
  }
};

}