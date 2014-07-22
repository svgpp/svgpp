// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

namespace svgpp { namespace factory { namespace color
{

template<class BaseFactory>
struct percentage_adapter: BaseFactory
{
  static unsigned char cast_percent(unsigned char c)
  {
    if (c > 100)
      c = 100;
    return (static_cast<unsigned int>(c) * 255 + 49) / 100;
  }

  static typename BaseFactory::color_type create_from_percent(unsigned char r, unsigned char g, unsigned char b)
  {
    return BaseFactory::create(cast_percent(r), cast_percent(g), cast_percent(b));
  }
};

struct rgb8_policy
{
  static const int preset_bits = 0;
  static const int r_offset = 16;
  static const int g_offset = 8;
  static const int b_offset = 0;
};

template<
  class Value = int, 
  class Policy = rgb8_policy
>
struct integer_base
{
  typedef Value color_type;

  static color_type create(unsigned char r, unsigned char g, unsigned char b)
  {
    return Policy::preset_bits 
      | (static_cast<color_type>(r) << Policy::r_offset)
      | (static_cast<color_type>(g) << Policy::g_offset)
      | (static_cast<color_type>(b) << Policy::b_offset);
  }
};

template<
  class Value = int, 
  class Policy = rgb8_policy
>
struct integer: 
  percentage_adapter<integer_base<Value, Policy> >
{};

}}}