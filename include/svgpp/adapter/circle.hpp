// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/detail/adapt_context.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/mpl/set.hpp>

namespace svgpp
{

typedef boost::mpl::set3<tag::attribute::cx, tag::attribute::cy, tag::attribute::r>
  circle_shape_attributes;

template<class Length>
class collect_circle_attributes_adapter: boost::noncopyable
{
public:
  template<class Context>
  bool on_exit_attributes(Context & context) const
  {
    typedef detail::unwrap_context<Context, tag::length_policy> length_policy_context;
    typedef typename length_policy_context::policy length_policy_t;

    typename length_policy_t::length_factory_type & converter 
      = length_policy_t::length_factory(length_policy_context::get(context));

    typename length_policy_t::length_factory_type::coordinate_type
      cx = 0, cy = 0, r = 0;
    if (cx_)
      cx = converter.length_to_user_coordinate(*cx_, tag::length_dimension::width());
    if (cy_)
      cy = converter.length_to_user_coordinate(*cy_, tag::length_dimension::height());
    if (r_) // required attribute
    {
      r = converter.length_to_user_coordinate(*r_, tag::length_dimension::not_width_nor_height());
      if (r < 0)
      {
        typedef detail::unwrap_context<Context, tag::error_policy> error_policy;
        return error_policy::policy::negative_value(error_policy::get(context), tag::attribute::r());
      }
        
      if (r == 0)
        return true;
      typedef detail::unwrap_context<Context, tag::basic_shapes_events_policy> basic_shapes_events;
      basic_shapes_events::policy::set_circle(basic_shapes_events::get(context), cx, cy, r);
    }
    return true;
  }

  void set(tag::attribute::cx, Length const & val) { cx_ = val; }
  void set(tag::attribute::cy, Length const & val) { cy_ = val; }
  void set(tag::attribute::r, Length const & val)  { r_ = val; }

private:
  boost::optional<Length> cx_, cy_, r_;
};

struct circle_to_path_adapter
{
  template<class Context, class Coordinate>
  static void set_circle(Context & context, Coordinate cx, Coordinate cy, Coordinate r)
  {
    typedef detail::unwrap_context<Context, tag::path_events_policy> path_events;

    typename path_events::type & path_context = path_events::get(context);
    path_events::policy::path_move_to(path_context, cx + r, cy, tag::coordinate::absolute());
    path_events::policy::path_elliptical_arc_to(path_context, 
      r, r, Coordinate(0), false, true, cx - r, cy, tag::coordinate::absolute());
    path_events::policy::path_elliptical_arc_to(path_context, 
      r, r, Coordinate(0), false, true, cx + r, cy, tag::coordinate::absolute());
    path_events::policy::path_close_subpath(path_context);
    path_events::policy::path_exit(path_context);
  }
};

}