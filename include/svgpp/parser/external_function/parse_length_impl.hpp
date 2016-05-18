// Copyright Oleg Maximenko 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/spirit/include/qi.hpp>
#include <svgpp/config.hpp>
#include <svgpp/parser/detail/common.hpp>
#include <svgpp/parser/external_function/parse_length.hpp>
#include <svgpp/parser/grammar/length.hpp>

#define SVGPP_PARSE_LENGTH_IMPL(IteratorType, LengthFactoryType) \
  template bool svgpp::detail::parse_length<svgpp::tag::length_dimension::width,                 svgpp::tag::source::attribute, LengthFactoryType, IteratorType> \
    (LengthFactoryType const &, IteratorType &, IteratorType, LengthFactoryType::length_type &); \
  template bool svgpp::detail::parse_length<svgpp::tag::length_dimension::height,                svgpp::tag::source::attribute, LengthFactoryType, IteratorType> \
    (LengthFactoryType const &, IteratorType &, IteratorType, LengthFactoryType::length_type &); \
  template bool svgpp::detail::parse_length<svgpp::tag::length_dimension::not_width_nor_height,  svgpp::tag::source::attribute, LengthFactoryType, IteratorType> \
    (LengthFactoryType const &, IteratorType &, IteratorType, LengthFactoryType::length_type &); \
  template bool svgpp::detail::parse_length<svgpp::tag::length_dimension::not_width_nor_height,  svgpp::tag::source::css,       LengthFactoryType, IteratorType> \
    (LengthFactoryType const &, IteratorType &, IteratorType, LengthFactoryType::length_type &); \
  \
  template bool svgpp::detail::parse_percentage_or_length<LengthFactoryType, IteratorType> \
    (LengthFactoryType const &, IteratorType &, IteratorType, LengthFactoryType::length_type &); \
  \
  template boost::spirit::qi::grammar< \
      IteratorType, LengthFactoryType::length_type(LengthFactoryType const &), \
      boost::spirit::qi::locals<LengthFactoryType::number_type> \
    > const & svgpp::detail::get_length_rule<svgpp::tag::length_dimension::width,                IteratorType, svgpp::tag::source::attribute, LengthFactoryType>(length_grammar_tag, LengthFactoryType const &); \
  template boost::spirit::qi::grammar< \
      IteratorType, LengthFactoryType::length_type(LengthFactoryType const &), \
      boost::spirit::qi::locals<LengthFactoryType::number_type> \
    > const & svgpp::detail::get_length_rule<svgpp::tag::length_dimension::height,               IteratorType, svgpp::tag::source::attribute, LengthFactoryType>(length_grammar_tag, LengthFactoryType const &); \
  template boost::spirit::qi::grammar< \
      IteratorType, LengthFactoryType::length_type(LengthFactoryType const &), \
      boost::spirit::qi::locals<LengthFactoryType::number_type> \
    > const & svgpp::detail::get_length_rule<svgpp::tag::length_dimension::not_width_nor_height, IteratorType, svgpp::tag::source::attribute, LengthFactoryType>(length_grammar_tag, LengthFactoryType const &); \
  template boost::spirit::qi::grammar< \
      IteratorType, LengthFactoryType::length_type(LengthFactoryType const &), \
      boost::spirit::qi::locals<LengthFactoryType::number_type> \
    > const & svgpp::detail::get_length_rule<svgpp::tag::length_dimension::not_width_nor_height, IteratorType, svgpp::tag::source::css,       LengthFactoryType>(length_grammar_tag, LengthFactoryType const &); 

namespace svgpp { namespace detail 
{

template<class Direction, class PropertySource, class LengthFactory, class Iterator>
bool parse_length(
  LengthFactory const & length_factory,
  Iterator & it, Iterator end,
  typename LengthFactory::length_type & out_length)
{
  SVGPP_STATIC_IF_SAFE const length_grammar<
    PropertySource,
    Iterator,
    LengthFactory,
    Direction
  > length_grammar;
  return boost::spirit::qi::parse(it, end, length_grammar(boost::phoenix::cref(length_factory)), out_length);
}

template<class LengthFactory, class Iterator>
bool parse_percentage_or_length(
  LengthFactory const & length_factory,
  Iterator & it, Iterator end,
  typename LengthFactory::length_type & out_length)
{
  SVGPP_STATIC_IF_SAFE const percentage_or_length_css_grammar<
    Iterator,
    LengthFactory
  > length_grammar;
  return boost::spirit::qi::parse(it, end, length_grammar(boost::phoenix::cref(length_factory)), out_length);
}

template<class Direction, class Iterator, class PropertySource, class LengthFactory>
boost::spirit::qi::grammar<
  Iterator,
  typename LengthFactory::length_type(LengthFactory const &),
  boost::spirit::qi::locals<typename LengthFactory::number_type>
> const & get_length_rule(length_grammar_tag, LengthFactory const &)
{
  static const length_grammar<
    PropertySource,
    Iterator,
    LengthFactory,
    Direction
  > length_grammar;
  return length_grammar;
}

template<class Direction, class Iterator, class PropertySource, class LengthFactory>
boost::spirit::qi::grammar<
  Iterator,
  typename LengthFactory::length_type(LengthFactory const &),
  boost::spirit::qi::locals<typename LengthFactory::number_type>
> const & get_length_rule(percentage_or_length_grammar_tag, LengthFactory const &)
{
  static const percentage_or_length_css_grammar<
    PropertySource,
    Iterator,
    LengthFactory
  > length_grammar;
  return length_grammar;
}

}}