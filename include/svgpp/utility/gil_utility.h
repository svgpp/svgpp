#pragma once

#include <boost/gil/channel.hpp>

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
    return channel_convert<GrayChannelValue>( bits32f(
      channel_convert<bits32f>(red  )*0.2125f + 
      channel_convert<bits32f>(green)*0.7154f + 
      channel_convert<bits32f>(blue )*0.0721f) );
  }
};

// performance specialization for unsigned char
template <typename GrayChannelValue>
struct rgb_to_luminance_fn<uint8_t,uint8_t,uint8_t, GrayChannelValue> 
{
  GrayChannelValue operator()(uint8_t red, uint8_t green, uint8_t blue) const 
  {
    return boost::gil::channel_convert<GrayChannelValue>(uint8_t(
      ((uint32_t(red)  *uint32_t(0.2125 * (1 << 14)) 
      + uint32_t(green)*uint32_t(0.7154 * (1 << 14)) 
      + uint32_t(blue) *uint32_t(0.0721 * (1 << 14)))) >> 14));
  }
};

template <typename GrayChannel, typename RedChannel, typename GreenChannel, typename BlueChannel>
inline typename boost::gil::channel_traits<GrayChannel>::value_type rgb_to_luminance(const RedChannel& red, const GreenChannel& green, const BlueChannel& blue) 
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

} // namespace gil_utility

}
