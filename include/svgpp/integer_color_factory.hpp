#pragma once

namespace svgpp
{

template<class BaseFactory>
struct color_factory_percentage_adapter: BaseFactory
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

struct rgb8_color_policy
{
  static const int preset_bits = 0;
  static const int r_offset = 16;
  static const int g_offset = 8;
  static const int b_offset = 0;
};

template<
  class Value = int, 
  class Policy = rgb8_color_policy
>
struct integer_color_factory_base
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
  class Policy = rgb8_color_policy
>
struct integer_color_factory: 
  color_factory_percentage_adapter<integer_color_factory_base<Value, Policy> >
{};

}