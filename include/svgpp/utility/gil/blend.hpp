// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/utility/gil/common.hpp>
#include <boost/gil/channel_algorithm.hpp>
#include <boost/gil/color_base_algorithm.hpp>

namespace svgpp 
{ 

namespace gil_detail 
{

namespace gil = boost::gil;

template<class BlendModeTag, class ChannelValue>
struct blend_channel_fn;

template<class ChannelValue>
struct blend_alpha_fn;

// TODO: default implementation for non-8 bit channels
// TODO: clamp_channels is not needed for premultiplied values

// For signed channels we call unsigned analog, converting forward and back
template<class BlendModeTag>
struct blend_channel_fn<BlendModeTag, boost::int8_t>
{
  boost::int8_t operator()(boost::int8_t channel_a, boost::int8_t channel_b, boost::int8_t alpha_a, boost::int8_t alpha_b) const
  {
    typedef gil::detail::channel_convert_to_unsigned<boost::int8_t> to_unsigned;
    typedef gil::detail::channel_convert_from_unsigned<boost::int8_t> from_unsigned;
    blend_channel_fn<BlendModeTag, boost::uint8_t> converter_unsigned;
    return from_unsigned()(converter_unsigned(
      to_unsigned()(channel_a), to_unsigned()(channel_b), to_unsigned()(alpha_a), to_unsigned()(alpha_b)));
  }
};

// normal	cr = (1 - alpha_a) * channel_b + channel_a
template<>
struct blend_channel_fn<tag::value::normal, boost::uint8_t>
{
  boost::uint8_t operator()(int channel_a, int channel_b, int alpha_a, int alpha_b) const
  {
    return clamp_channel_bits8(((255 - alpha_a) * channel_b) / 255 + channel_a);
  }
};

// multiply	cr = (1-alpha_a)*channel_b + (1-alpha_b)*channel_a + channel_a*channel_b
template<>
struct blend_channel_fn<tag::value::multiply, boost::uint8_t>
{
  boost::uint8_t operator()(int channel_a, int channel_b, int alpha_a, int alpha_b) const
  {
    return clamp_channel_bits8(((255 - alpha_a) * channel_b + (255 - alpha_b) * channel_a + channel_a * channel_b) / 255);
  }
};

// screen	cr = channel_b + channel_a - channel_a * channel_b
template<>
struct blend_channel_fn<tag::value::screen, boost::uint8_t>
{
  boost::uint8_t operator()(int channel_a, int channel_b, int alpha_a, int alpha_b) const
  {
    return clamp_channel_bits8(channel_b + channel_a - channel_a * channel_b / 255);
  }
};

// darken	cr = Min ((1 - alpha_a) * channel_b + channel_a, (1 - alpha_b) * channel_a + channel_b)
template<>
struct blend_channel_fn<tag::value::darken, boost::uint8_t>
{
  boost::uint8_t operator()(int channel_a, int channel_b, int alpha_a, int alpha_b) const
  {
    return clamp_channel_bits8(std::min((255 - alpha_a) * channel_b / 255 + channel_a, (255 - alpha_b) * channel_a / 255 + channel_b));
  }
};

// lighten	cr = Max ((1 - alpha_a) * channel_b + channel_a, (1 - alpha_b) * channel_a + channel_b)
template<>
struct blend_channel_fn<tag::value::lighten, boost::uint8_t>
{
  boost::uint8_t operator()(int channel_a, int channel_b, int alpha_a, int alpha_b) const
  {
    return clamp_channel_bits8(std::max((255 - alpha_a) * channel_b / 255 + channel_a, (255 - alpha_b) * channel_a / 255 + channel_b));
  }
};

// qr = 1 - (1-alpha_a)*(1-alpha_b)
template<>
struct blend_alpha_fn<boost::uint8_t>
{
  boost::uint8_t operator()(int alpha_a, int alpha_b) const
  {
    return clamp_channel_bits8(255 - (255 - alpha_a) * (255 - alpha_b) / 255);
  }
};

} // namespace gil_detail 

namespace gil_utility 
{
  
template<class BlendModeTag>
struct blend_pixel
{
  template<class Color>
  Color operator()(const Color & pixa, const Color & pixb) const 
  {
    namespace gil = boost::gil;

    typename gil::color_element_type<Color, gil::alpha_t>::type 
      alpha_a = gil::get_color(pixa, gil::alpha_t()),
      alpha_b = gil::get_color(pixb, gil::alpha_t());

    Color result;

    gil::get_color(result, gil::red_t()) 
      = gil_detail::blend_channel_fn<BlendModeTag, typename gil::color_element_type<Color, gil::red_t>::type>()(
        gil::get_color(pixa, gil::red_t()), gil::get_color(pixb, gil::red_t()),
        alpha_a, alpha_b);

    gil::get_color(result, gil::green_t()) 
      = gil_detail::blend_channel_fn<BlendModeTag, typename gil::color_element_type<Color, gil::green_t>::type>()(
        gil::get_color(pixa, gil::green_t()), gil::get_color(pixb, gil::green_t()),
        alpha_a, alpha_b);

    gil::get_color(result, gil::blue_t()) 
      = gil_detail::blend_channel_fn<BlendModeTag, typename gil::color_element_type<Color, gil::blue_t>::type>()(
        gil::get_color(pixa, gil::blue_t()), gil::get_color(pixb, gil::blue_t()),
        alpha_a, alpha_b);

    gil::get_color(result, gil::alpha_t()) = 
      gil_detail::blend_alpha_fn<typename gil::color_element_type<Color, gil::alpha_t>::type>()(alpha_a, alpha_b);

    return result;
  }
};

}}
