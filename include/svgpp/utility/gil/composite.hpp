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

template<class CompositeModeTag, class ChannelValue>
struct composite_channel_fn;

template<class CompositeModeTag, class ChannelValue>
struct composite_alpha_fn;

template<class ChannelValue>
struct composite_arithmetic_channel_fn;

// TODO: default implementation for non-8 bit channels

// For signed channels we call unsigned analog, converting forward and back
template<class CompositeModeTag>
struct composite_channel_fn<CompositeModeTag, gil::bits8s>
{
  gil::bits8s operator()(gil::bits8s channel_a, gil::bits8s channel_b, gil::bits8s alpha_a, gil::bits8s alpha_b) const
  {
    typedef gil::detail::channel_convert_to_unsigned<gil::bits8s> to_unsigned;
    typedef gil::detail::channel_convert_from_unsigned<gil::bits8s> from_unsigned;
    composite_channel_fn<CompositeModeTag, gil::bits8> converter_unsigned;
    return from_unsigned()(converter_unsigned(
      to_unsigned()(channel_a), to_unsigned()(channel_b), to_unsigned()(alpha_a), to_unsigned()(alpha_b)));
  }
};

// Dca' = Sca + Dca x (1 - Sa)
template<>
struct composite_channel_fn<tag::value::over, gil::bits8>
{
  gil::bits8 operator()(int channel_a, int channel_b, int alpha_a, int alpha_b) const
  {
    return clamp_channel_bits8(channel_a + channel_b * (255 - alpha_a) / 255);
  }
};

// Da'  = Sa + Da - Sa x Da
template<>
struct composite_alpha_fn<tag::value::over, gil::bits8>
{
  gil::bits8 operator()(int alpha_a, int alpha_b) const
  {
    return clamp_channel_bits8(alpha_a + alpha_b - alpha_a * alpha_b / 255);
  }
};

// Dca' = Sca x Da
template<>
struct composite_channel_fn<tag::value::in, gil::bits8>
{
  gil::bits8 operator()(int channel_a, int channel_b, int alpha_a, int alpha_b) const
  {
    return channel_a * alpha_b / 255;
  }
};

// Da'  = Sa x Da
template<>
struct composite_alpha_fn<tag::value::in, gil::bits8>
{
  gil::bits8 operator()(int alpha_a, int alpha_b) const
  {
    return alpha_a * alpha_b / 255;
  }
};

// Dca' = Sca x (1 - Da)
template<>
struct composite_channel_fn<tag::value::out, gil::bits8>
{
  gil::bits8 operator()(int channel_a, int channel_b, int alpha_a, int alpha_b) const
  {
    return channel_a * alpha_a * (255 - alpha_b) / 65535;
  }
};

// Da'  = Sa x (1 - Da)
template<>
struct composite_alpha_fn<tag::value::out, gil::bits8>
{
  gil::bits8 operator()(int alpha_a, int alpha_b) const
  {
    return alpha_a * (255 - alpha_b) / 255;
  }
};

// Dca' = Sca x Da + Dca x (1 - Sa)
template<>
struct composite_channel_fn<tag::value::atop, gil::bits8>
{
  gil::bits8 operator()(int channel_a, int channel_b, int alpha_a, int alpha_b) const
  {
    return (channel_a * alpha_a + channel_b * (255 - alpha_a)) * alpha_b / 65535;
  }
};

// Da'  = Da
template<>
struct composite_alpha_fn<tag::value::atop, gil::bits8>
{
  gil::bits8 operator()(gil::bits8 alpha_a, gil::bits8 alpha_b) const
  {
    return alpha_b;
  }
};

// Dca' = Sca x (1 - Da) + Dca x (1 - Sa)
template<>
struct composite_channel_fn<tag::value::xor_, gil::bits8>
{
  gil::bits8 operator()(int channel_a, int channel_b, int alpha_a, int alpha_b) const
  {
    return clamp_channel_bits8((channel_a * alpha_a * (255 - alpha_b) + channel_b * alpha_b * (255 - alpha_a)) / 65535);
  }
};

// Da'  = Sa + Da - 2 x Sa x Da
template<>
struct composite_alpha_fn<tag::value::xor_, gil::bits8>
{
  gil::bits8 operator()(int alpha_a, int alpha_b) const
  {
    return clamp_channel_bits8((alpha_a + alpha_b - 2 * alpha_a * alpha_b) / 255);
  }
};

// result = k1*i1*i2 + k2*i1 + k3*i2 + k4
template<>
struct composite_arithmetic_channel_fn<gil::bits8>
{
  template<class Coefficient>
  composite_arithmetic_channel_fn(Coefficient k1, Coefficient k2, Coefficient k3, Coefficient k4)
    : k1_(k1 * 255), k2_(k2 * 255), k3_(k3 * 255), k4_(k4 * 255)
  {}

  gil::bits8 operator()(int channel_a, int channel_b) const 
  {
    return clamp_channel_bits8(k1_ * channel_a * channel_b / 65535 + k2_ * channel_a / 255 + k3_ * channel_b / 255 + k4_);
  }

private:
  int k1_, k2_, k3_, k4_;
};

} // namespace gil_detail 

namespace gil_utility 
{

namespace gil = boost::gil;
  
template<class CompositeModeTag>
struct composite_pixel
{
  template<class Color>
  Color operator()(const Color & pixa, const Color & pixb) const 
  {
    typename gil::color_element_type<Color, gil::alpha_t>::type 
      alpha_a = gil::get_color(pixa, gil::alpha_t()),
      alpha_b = gil::get_color(pixb, gil::alpha_t());

    Color result;

    gil::get_color(result, gil::red_t()) 
      = gil_detail::composite_channel_fn<CompositeModeTag, typename gil::color_element_type<Color, gil::red_t>::type>()(
        gil::get_color(pixa, gil::red_t()), gil::get_color(pixb, gil::red_t()),
        alpha_a, alpha_b);

    gil::get_color(result, gil::green_t()) 
      = gil_detail::composite_channel_fn<CompositeModeTag, typename gil::color_element_type<Color, gil::green_t>::type>()(
        gil::get_color(pixa, gil::green_t()), gil::get_color(pixb, gil::green_t()),
        alpha_a, alpha_b);

    gil::get_color(result, gil::blue_t()) 
      = gil_detail::composite_channel_fn<CompositeModeTag, typename gil::color_element_type<Color, gil::blue_t>::type>()(
        gil::get_color(pixa, gil::blue_t()), gil::get_color(pixb, gil::blue_t()),
        alpha_a, alpha_b);

    gil::get_color(result, gil::alpha_t()) = 
      gil_detail::composite_alpha_fn<CompositeModeTag, typename gil::color_element_type<Color, gil::alpha_t>::type>()(alpha_a, alpha_b);

    return result;
  }
};

template<class Color>
struct composite_pixel_arithmetic
{
  template<class Coefficient>
  composite_pixel_arithmetic(Coefficient k1, Coefficient k2, Coefficient k3, Coefficient k4)
    : r_(k1, k2, k3, k4)
    , g_(k1, k2, k3, k4)
    , b_(k1, k2, k3, k4)
    , a_(k1, k2, k3, k4)
  {}

  Color operator()(const Color & pixa, const Color & pixb) const 
  {
    Color result;
    gil::get_color(result, gil::red_t())   = r_(gil::get_color(pixa, gil::red_t())   , gil::get_color(pixb, gil::red_t())   );
    gil::get_color(result, gil::green_t()) = r_(gil::get_color(pixa, gil::green_t()) , gil::get_color(pixb, gil::green_t()) );
    gil::get_color(result, gil::blue_t())  = r_(gil::get_color(pixa, gil::blue_t())  , gil::get_color(pixb, gil::blue_t())  );
    gil::get_color(result, gil::alpha_t()) = r_(gil::get_color(pixa, gil::alpha_t()) , gil::get_color(pixb, gil::alpha_t()) );
    return result;
  }

private:
  gil_detail::composite_arithmetic_channel_fn<typename gil::color_element_type<Color, gil::red_t  >::type> r_;
  gil_detail::composite_arithmetic_channel_fn<typename gil::color_element_type<Color, gil::green_t>::type> g_;
  gil_detail::composite_arithmetic_channel_fn<typename gil::color_element_type<Color, gil::blue_t >::type> b_;
  gil_detail::composite_arithmetic_channel_fn<typename gil::color_element_type<Color, gil::alpha_t>::type> a_;
};

}}
