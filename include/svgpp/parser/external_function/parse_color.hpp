// Copyright Oleg Maximenko 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>
#include <svgpp/definitions.hpp>

namespace svgpp { namespace detail 
{

template<class ColorFactory, class Iterator, class PropertySource>
bool parse_color(
  Iterator & it, Iterator end, PropertySource property_source, 
  typename ColorFactory::color_type & out_color);

template<class ColorFactory, class IccColorFactory, class Iterator, class PropertySource>
bool parse_color_optional_icc_color(
  IccColorFactory const & icc_color_factory,
  Iterator & it, Iterator end, PropertySource property_source,
  boost::tuple<
    typename ColorFactory::color_type, boost::optional<typename IccColorFactory::icc_color_type>
  > & out_color);

}}