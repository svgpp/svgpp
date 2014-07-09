#pragma once

#include <svgpp/context_policy_load_path.hpp>
#include <svgpp/context_policy_load_value.hpp>
#include <svgpp/definitions.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/mpl/set.hpp>
#include <stdexcept>

namespace svgpp
{

typedef boost::mpl::set3<tag::attribute::cx, tag::attribute::cy, tag::attribute::r>
  circle_shape_attributes;

template<class Length>
class collect_circle_attributes_adapter: boost::noncopyable
{
public:
  template<class Context, class LengthToUserCoordinatesConverter>
  void on_exit_attributes(Context & context, LengthToUserCoordinatesConverter const & converter) const
  {
    on_exit_attributesT<context_policy<tag::load_value_policy, Context> >(context, converter);
  }

  template<class ErrorPolicy, class LoadPolicy, class Context, class LengthToUserCoordinatesConverter>
  bool on_exit_attributesT(Context & context, LengthToUserCoordinatesConverter const & converter) const
  {
    typename LengthToUserCoordinatesConverter::coordinate_type
      cx = 0, cy = 0, r = 0;
    if (cx_)
      cx = converter.length_to_user_coordinate(*cx_, tag::width_length());
    if (cy_)
      cy = converter.length_to_user_coordinate(*cy_, tag::height_length());
    if (r_) // required attribute
    {
      r = converter.length_to_user_coordinate(*r_, tag::not_width_nor_height_length());
      if (r < 0)
        return ErrorPolicy::negative_value(context, tag::attribute::r());
      if (r == 0)
        return true;
      LoadPolicy::set_circle(context, cx, cy, r);
    }
    return true;
  }

  void set(tag::attribute::cx, Length const & val) { cx_ = val; }
  void set(tag::attribute::cy, Length const & val) { cy_ = val; }
  void set(tag::attribute::r, Length const & val)  { r_ = val; }

private:
  boost::optional<Length> cx_, cy_, r_;
};

template<class LoadPathPolicy = void>
struct circle_to_path_adapter
{
  template<class Context, class Coordinate>
  static void set_circle(Context & context, Coordinate cx, Coordinate cy, Coordinate r)
  {
    typedef typename boost::mpl::if_<
      boost::is_same<LoadPathPolicy, void>,
      context_policy<tag::load_path_policy, Context>,
      LoadPathPolicy
    >::type load_policy;

    load_policy::path_move_to(context, cx + r, cy, tag::absolute_coordinate());
    load_policy::path_elliptical_arc_to(context, r, r, 0, false, true, cx - r, cy, tag::absolute_coordinate());
    load_policy::path_elliptical_arc_to(context, r, r, 0, false, true, cx + r, cy, tag::absolute_coordinate());
    load_policy::path_close_subpath(context);
    load_policy::path_exit(context);
  }
};

}