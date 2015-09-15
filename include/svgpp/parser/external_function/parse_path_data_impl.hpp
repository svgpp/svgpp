// Copyright Oleg Maximenko 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/parser/grammar/path_data.hpp>
#include <svgpp/parser/external_function/parse_path_data.hpp>

#define SVGPP_PARSE_PATH_DATA_IMPL(IteratorType, CoordinateType) \
  template bool svgpp::detail::parse_path_data<IteratorType, CoordinateType>( \
    IteratorType &, IteratorType, svgpp::detail::path_events_interface<CoordinateType> &);

namespace svgpp { namespace detail 
{

template<class Iterator, class Coordinate>
bool parse_path_data(Iterator & it, Iterator end, path_events_interface<Coordinate> & context)
{
  typedef path_data_grammar<
      Iterator, 
      path_events_interface<Coordinate>,
      Coordinate,
      policy::path_events::forward_to_method<path_events_interface<Coordinate> >
    > path_data_grammar_t;

  SVGPP_STATIC_IF_SAFE const path_data_grammar_t grammar;
  return qi::phrase_parse(it, end, grammar(boost::phoenix::ref(context)), 
    typename path_data_grammar_t::skipper_type());
}

}}