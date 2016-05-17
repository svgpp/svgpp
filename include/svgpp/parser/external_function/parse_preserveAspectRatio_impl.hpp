// Copyright Oleg Maximenko 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/parser/external_function/parse_preserveAspectRatio.hpp>

#define SVGPP_PARSE_PRESERVE_ASPECT_RATIO_IMPL(IteratorType) \
  template bool svgpp::detail::parse_preserveAspectRatio<IteratorType>( \
    IteratorType &, IteratorType, svgpp::preserveAspectRatio_value &);

namespace svgpp { namespace detail 
{

template<class Iterator>
bool parse_preserveAspectRatio(Iterator & it, Iterator end, preserveAspectRatio_value & value)
{
  SVGPP_STATIC_IF_SAFE const preserveAspectRatio_grammar<Iterator> grammar;
  return boost::spirit::qi::parse(it, end, grammar, value);
}

}}