// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/range/iterator_range.hpp>
#include <boost/noncopyable.hpp>

namespace svgpp { 

namespace tag
{
  struct skip_icc_color {};
}
  
namespace factory { namespace icc_color
{

struct stub: boost::noncopyable
{
  typedef double component_type;
  typedef tag::skip_icc_color icc_color_type;
  struct builder_type {};

  stub() {}

  template<class Iterator>
  static void set_profile_name(builder_type &, typename boost::iterator_range<Iterator> const &) {}
  static void append_component_value(builder_type &, component_type) {}

  static icc_color_type create_icc_color(builder_type const &) { return icc_color_type(); }
};

}}}
