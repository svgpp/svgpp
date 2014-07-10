#pragma once

#include <svgpp/factory/icc_color_stub.hpp>
#include <svgpp/factory/integer_color.hpp>

namespace svgpp { namespace factory
{

  namespace color
  {
    typedef integer<> default_factory;

    template<class Context>
    struct by_context
    {
      typedef default_factory type;
    };
  }

  namespace icc_color
  {
    typedef stub default_factory;

    template<class Context>
    struct by_context
    {
      typedef default_factory type;
    };
  }

}}