// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/detail/names_dictionary.hpp>

namespace svgpp { namespace policy { namespace css_name_to_id
{

struct default_policy
{
  template<class Range>
  static detail::attribute_id find(Range const & str)
  {
    return detail::css_property_name_to_id_dictionary::find_ignore_case(str);
  }
};

struct case_sensitive
{
  template<class Range>
  static detail::attribute_id find(Range const & str)
  {
    return detail::css_property_name_to_id_dictionary::find(str);
  }
};

}}}