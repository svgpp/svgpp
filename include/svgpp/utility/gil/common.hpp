// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/cstdint.hpp>
#include <boost/gil/typedefs.hpp>

namespace svgpp { namespace gil_detail 
{

inline boost::uint8_t clamp_channel_bits8(int c)
{
  if (c > 255)
    return 255;
  if (c < 0)
    return 0;
  return static_cast<boost::uint8_t>(c);
}

}}