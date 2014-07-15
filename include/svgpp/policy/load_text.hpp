#pragma once

#include <svgpp/policy/detail/same_object_func.hpp>

namespace svgpp { namespace policy { namespace load_text 
{

template<class Context>
struct forward_to_method
{
  template<class T>
  static void set_text(Context & context, T const & value)
  {
    context.set_text(value);
  }
};

template<class Context>
struct default_policy: forward_to_method<Context>
{};

}}}