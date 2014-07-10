#pragma once

#include <svgpp/factory/unitless_angle.hpp>

namespace svgpp { namespace factory { namespace angle 
{

  typedef unitless<> default_factory;

  template<class Context>
  struct by_context
  {
    typedef default_factory type;
  };

}}}