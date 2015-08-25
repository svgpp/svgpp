// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/traits/child_element_types.hpp>
#include <boost/assert.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/has_key.hpp>
#include <boost/utility/enable_if.hpp>

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4127) // conditional expression is constant
#endif

namespace svgpp { namespace detail 
{

template<class ElementTag, class F, bool Enabled>
struct call_if_enabled
{
  static bool call(F & fn)
  {
    fn(ElementTag());
    return true;
  }
};

template<class ElementTag, class F>
struct call_if_enabled<ElementTag, F, false>
{
  BOOST_CONSTEXPR static bool call(F const &)
  {
    return false;
  }
};

template<class SupportedElements, class F>
bool id_to_element_tag(element_type_id id, F & fn)
{
  switch (id)
  {
#define SVGPP_ON(element_name, str) \
  case element_type_id_## element_name: \
    if (boost::mpl::has_key<SupportedElements, tag::element::element_name>::value) \
      return detail::call_if_enabled<tag::element::element_name, F, \
        boost::mpl::has_key<SupportedElements, tag::element::element_name>::value > \
      ::call(fn); \
    break;
#include <svgpp/detail/dict/enumerate_all_elements.inc>
#undef SVGPP_ON
  default:
    BOOST_ASSERT(false);
    break;
  }
  return false;
}

}}

#ifdef _MSC_VER
# pragma warning(pop)
#endif
