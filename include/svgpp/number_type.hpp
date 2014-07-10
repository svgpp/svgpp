#pragma once

namespace svgpp 
{

  typedef double default_number_type;

  template<class Context>
  struct number_type_by_context
  {
    typedef default_number_type type;
  };

}