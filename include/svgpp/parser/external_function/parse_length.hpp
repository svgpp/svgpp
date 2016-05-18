// Copyright Oleg Maximenko 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <boost/spirit/include/qi.hpp>

namespace svgpp { namespace detail 
{

template<class Direction, class PropertySource, class LengthFactory, class Iterator>
bool parse_length(
  LengthFactory const & length_factory,
  Iterator & it, Iterator end,
  typename LengthFactory::length_type & out_length);

template<class LengthFactory, class Iterator>
bool parse_percentage_or_length(
  LengthFactory const & length_factory,
  Iterator & it, Iterator end,
  typename LengthFactory::length_type & out_length);

struct length_grammar_tag {};
struct percentage_or_length_grammar_tag {};

template<class Direction, class Iterator, class PropertySource, class LengthFactory>
boost::spirit::qi::grammar<
  Iterator, 
  typename LengthFactory::length_type(LengthFactory const &), 
  boost::spirit::qi::locals<typename LengthFactory::number_type> 
> const & get_length_rule(length_grammar_tag, LengthFactory const &);

template<class Direction, class Iterator, class PropertySource, class LengthFactory>
boost::spirit::qi::grammar<
  Iterator,
  typename LengthFactory::length_type(LengthFactory const &),
  boost::spirit::qi::locals<typename LengthFactory::number_type>
> const & get_length_rule(percentage_or_length_grammar_tag, LengthFactory const &);

}}