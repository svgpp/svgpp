#pragma once

namespace svgpp { namespace detail 
{

template<class Object>
struct same_object_func
{
  static Object & call(Object & val)
  {
    return val;
  }
};

}}