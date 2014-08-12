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

typedef boost::mpl::set4<tag::attribute::x1, tag::attribute::y1, tag::attribute::x2, tag::attribute::y2>
  line_shape_attributes;

template<class Length>
class collect_line_attributes_adapter: boost::noncopyable
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
      x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    if (x1_)
      x1 = converter.length_to_user_coordinate(*x1_, tag::length_dimension::width());
    if (x2_)
      x2 = converter.length_to_user_coordinate(*x2_, tag::length_dimension::width());
    if (y1_)
      y1 = converter.length_to_user_coordinate(*y1_, tag::length_dimension::height());
    if (y2_)
      y2 = converter.length_to_user_coordinate(*y2_, tag::length_dimension::height());

    typedef detail::unwrap_context<Context, tag::load_basic_shapes_policy> load_basic_shapes;
    load_basic_shapes::policy::set_line(load_basic_shapes::get(context), x1, y1, x2, y2);
    return true;
  }

  void set(tag::attribute::x1, Length const & val) { x1_ = val; }
  void set(tag::attribute::y1, Length const & val) { y1_ = val; }
  void set(tag::attribute::x2, Length const & val) { x2_ = val; }
  void set(tag::attribute::y2, Length const & val) { y2_ = val; }

private:
  boost::optional<Length> x1_, y1_, x2_, y2_;
};

struct line_to_path_adapter
{
  template<class Context, class Coordinate>
  static void set_line(Context & context, Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2)
  {
    typedef detail::unwrap_context<Context, tag::load_path_policy> load_path;

    typename load_path::type & path_context = load_path::get(context);
    load_path::policy::path_move_to(path_context, x1, y1, tag::absolute_coordinate());
    load_path::policy::path_line_to(path_context, x2, y2, tag::absolute_coordinate());
    load_path::policy::path_exit(path_context);
  }
};

}