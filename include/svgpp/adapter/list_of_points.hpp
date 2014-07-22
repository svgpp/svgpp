// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>

namespace svgpp
{

template<class OutputContext>
class list_of_points_to_path_adapter
{
public:
  list_of_points_to_path_adapter(OutputContext & original_context)
    : context_(original_context)
    , first_point_(true)
  {
  }

  template<class Coordinate>
  void append_list_point(Coordinate x, Coordinate y)
  {
    if (first_point_)
    {
      first_point_ = false;
      context_.path_move_to(x, y, tag::absolute_coordinate());
    }
    else
      context_.path_line_to(x, y, tag::absolute_coordinate());
  }

private:
  OutputContext & context_;
  bool first_point_;
};

}