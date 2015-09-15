// Copyright Oleg Maximenko 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/parser/grammar/transform.hpp>
#include <svgpp/parser/external_function/parse_transform.hpp>

#define SVGPP_PARSE_TRANSFORM_IMPL(IteratorType, NumberType) \
  template bool svgpp::detail::parse_transform<IteratorType, NumberType>( \
    IteratorType &, IteratorType, svgpp::detail::transform_events_interface<NumberType> &);

namespace svgpp { namespace detail 
{

template<class Iterator, class Number>
bool parse_transform(Iterator & it, Iterator end, transform_events_interface<Number> & context)
{
  typedef transform_grammar<
      Iterator, 
      transform_events_interface<Number>,
      Number,
      policy::transform_events::forward_to_method<transform_events_interface<Number> >
    > transform_grammar_t;

  SVGPP_STATIC_IF_SAFE const transform_grammar_t grammar;
  return qi::phrase_parse(it, end, grammar(boost::phoenix::ref(context)), 
    typename transform_grammar_t::skipper_type());
}

}}