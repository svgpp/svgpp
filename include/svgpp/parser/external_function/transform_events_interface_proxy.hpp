// Copyright Oleg Maximenko 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/parser/external_function/parse_transform.hpp>

namespace svgpp { namespace detail 
{

template<
  class Context,
  class Number,
  class EventsPolicy 
>
class transform_events_interface_proxy: public transform_events_interface<Number>
{
public:
  transform_events_interface_proxy(Context & context)
    : m_context(context)
  {}

  virtual void transform_matrix(const boost::array<Number, 6> & matrix)
  {
    EventsPolicy::transform_matrix(m_context, matrix);
  }

  virtual void transform_translate(Number tx, Number ty)
  {
    EventsPolicy::transform_translate(m_context, tx, ty);
  }

  virtual void transform_translate(Number tx)
  {
    EventsPolicy::transform_translate(m_context, tx);
  }

  virtual void transform_scale(Number sx, Number sy)
  {
    EventsPolicy::transform_scale(m_context, sx, sy);
  }

  virtual void transform_scale(Number scale)
  {
    EventsPolicy::transform_scale(m_context, scale);
  }

  virtual void transform_rotate(Number angle)
  {
    EventsPolicy::transform_rotate(m_context, angle);
  }

  virtual void transform_rotate(Number angle, Number cx, Number cy)
  {
    EventsPolicy::transform_rotate(m_context, angle, cx, cy);
  }

  virtual void transform_skew_x(Number angle)
  {
    EventsPolicy::transform_skew_x(m_context, angle);
  }

  virtual void transform_skew_y(Number angle)
  {
    EventsPolicy::transform_skew_y(m_context, angle);
  }

private:
  Context & m_context;
};

}}