// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/detail/adapt_context.hpp>

namespace svgpp
{

template<class ElementTag>
struct list_of_points_to_path_adapter;

template<>
struct list_of_points_to_path_adapter<tag::element::polyline>
{
  template<class Context, class Range>
  static void set(Context & context, tag::attribute::points, tag::source::attribute, Range const & r)
  {
    typedef detail::unwrap_context<Context, tag::path_events_policy> path_events;

    typename path_events::type & path_context = path_events::get(context);
    typename boost::range_const_iterator<Range>::type it = boost::begin(r), end = boost::end(r);
    if (it != end)
    {
      path_events::policy::path_move_to(path_context, it->first, it->second, tag::coordinate::absolute());
      for(++it; it != end; ++it)
        path_events::policy::path_line_to(path_context, it->first, it->second, tag::coordinate::absolute());
      path_events::policy::path_exit(path_context);
    }
  }
};

template<>
struct list_of_points_to_path_adapter<tag::element::polygon>
{
  template<class Context, class Range>
  static void set(Context & context, tag::attribute::points, tag::source::attribute, Range const & r)
  {
    typedef detail::unwrap_context<Context, tag::path_events_policy> path_events;

    typename path_events::type & path_context = path_events::get(context);
    typename boost::range_const_iterator<Range>::type it = boost::begin(r), end = boost::end(r);
    if (it != end)
    {
      path_events::policy::path_move_to(path_context, it->first, it->second, tag::coordinate::absolute());
      bool line = false;
      for(++it; it != end; ++it)
      {
        path_events::policy::path_line_to(path_context, it->first, it->second, tag::coordinate::absolute());
        line = true;
      }
      if (line)
      {
        path_events::policy::path_close_subpath(path_context);
        path_events::policy::path_exit(path_context);
      }
    }
  }
};

}