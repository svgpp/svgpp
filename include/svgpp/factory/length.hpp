#pragma once

#include <svgpp/factory/unitless_length.hpp>

namespace svgpp { namespace factory { namespace length 
{

  typedef unitless<> default_factory;

  template<class Context>
  struct by_context
  {
    typedef default_factory type;
  };

}}}