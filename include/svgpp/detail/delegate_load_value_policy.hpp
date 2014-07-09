#pragma once

namespace svgpp { namespace detail 
{

template<class OriginalPolicy, class GetOriginalContextFunc>
struct delegate_load_value_policy
{
  template<class Context, class Coordinate>
  static void set_viewport(Context & context, Coordinate viewport_x, Coordinate viewport_y, 
    Coordinate viewport_width, Coordinate viewport_height)
  {
    OriginalPolicy::set_viewport(GetOriginalContextFunc::get_original_context(context), 
      viewport_x, viewport_y, viewport_width, viewport_height);
  }

  template<class Context, class Coordinate>
  static void get_reference_viewport_size(Context & context, Coordinate & viewport_width, Coordinate & viewport_height)
  {
    OriginalPolicy::get_reference_viewport_size(GetOriginalContextFunc::get_original_context(context), 
      viewport_width, viewport_height);
  }
};

}}