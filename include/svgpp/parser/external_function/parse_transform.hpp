// Copyright Oleg Maximenko 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/array.hpp>
#include <svgpp/definitions.hpp>

namespace svgpp { namespace detail 
{

template<class Number>
struct transform_events_interface
{
  virtual void transform_matrix(const boost::array<Number, 6> & matrix) = 0;
  virtual void transform_translate(Number tx, Number ty) = 0;
  virtual void transform_translate(Number tx) = 0;
  virtual void transform_scale(Number sx, Number sy) = 0;
  virtual void transform_scale(Number scale) = 0;
  virtual void transform_rotate(Number angle) = 0;
  virtual void transform_rotate(Number angle, Number cx, Number cy) = 0;
  virtual void transform_skew_x(Number angle) = 0;
  virtual void transform_skew_y(Number angle) = 0;
};

template<class Iterator, class Number>
bool parse_transform(Iterator & it, Iterator end, transform_events_interface<Number> & context);

}}