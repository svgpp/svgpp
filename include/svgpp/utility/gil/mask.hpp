// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/cstdint.hpp>
#include <boost/gil/channel_algorithm.hpp>
#include <boost/gil/gray.hpp>
#include <boost/gil/rgb.hpp>
#include <boost/gil/rgba.hpp>

namespace svgpp 
{ 

namespace gil_detail 
{

/// red * 0.2125 + green * 0.7154 + blue * 0.0721

// The default implementation of to_luminance uses float0..1 as the intermediate channel type
template <typename RedChannel, typename GreenChannel, typename BlueChannel, typename GrayChannelValue>
struct rgb_to_luminance_fn 
{
  GrayChannelValue operator()(const RedChannel& red, const GreenChannel& green, const BlueChannel& blue) const 
  {
    using namespace boost::gil;
    return channel_convert<GrayChannelValue>(
      channel_convert<float32_t>(red  )*0.2125f +
      channel_convert<float32_t>(green)*0.7154f +
      channel_convert<float32_t>(blue )*0.0721f);
  }
};

// performance specialization for unsigned char
template <typename GrayChannelValue>
struct rgb_to_luminance_fn<boost::uint8_t, boost::uint8_t, boost::uint8_t, GrayChannelValue> 
{
  GrayChannelValue operator()(boost::uint8_t red, boost::uint8_t green, boost::uint8_t blue) const 
  {
    return boost::gil::channel_convert<GrayChannelValue>(boost::uint8_t(
       (boost::uint32_t(red)   * boost::uint32_t(0.2125 * (1 << 14)) 
      + boost::uint32_t(green) * boost::uint32_t(0.7154 * (1 << 14)) 
      + boost::uint32_t(blue)  * boost::uint32_t(0.0721 * (1 << 14))) >> 14));
  }
};

template <typename GrayChannel, typename RedChannel, typename GreenChannel, typename BlueChannel>
inline typename boost::gil::channel_traits<GrayChannel>::value_type rgb_to_luminance(
  const RedChannel& red, const GreenChannel& green, const BlueChannel& blue) 
{
  return rgb_to_luminance_fn<RedChannel,GreenChannel,BlueChannel,
                              typename boost::gil::channel_traits<GrayChannel>::value_type>()(red,green,blue);
}

}   // namespace gil_detail

namespace gil_utility 
{

template<class DestChannel = boost::gil::gray_color_t>
struct rgb_to_luminance_color_converter 
{
  template <typename P1, typename P2>
  void operator()(const P1& src, P2& dst) const 
  {
    using namespace boost::gil;
    get_color(dst, DestChannel()) = 
      gil_detail::rgb_to_luminance<typename color_element_type<P2, DestChannel>::type>(
          get_color(src, red_t()), 
          get_color(src, green_t()), 
          get_color(src, blue_t())
      );
  }
};

template<class DestChannel = boost::gil::gray_color_t>
struct rgba_to_mask_color_converter 
{
  template <typename P1, typename P2>
  void operator()(const P1& src, P2& dst) const 
  {
    using namespace boost::gil;
    get_color(dst, DestChannel()) = 
      boost::gil::channel_convert<typename color_element_type<P2, DestChannel>::type>(
        channel_multiply(
          gil_detail::rgb_to_luminance<typename color_element_type<P1, alpha_t>::type>(
              get_color(src, red_t()), 
              get_color(src, green_t()), 
              get_color(src, blue_t())
          ),
          get_color(src, alpha_t())
        )
      );
  }
};

}}
