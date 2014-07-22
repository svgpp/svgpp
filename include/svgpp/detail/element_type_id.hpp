// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

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