#pragma once

namespace svgpp { namespace detail
{

  enum element_type_id
  {
#define SVGPP_ON(element_name, str) element_type_id_## element_name,
#include <svgpp/detail/dict/enumerate_all_elements.inc>
#undef SVGPP_ON
    element_type_count,
    unknown_element_type_id = element_type_count
  };

}}