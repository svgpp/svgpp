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
#include <svgpp/parser/external_function/parse_misc.hpp>
#include <svgpp/parser/grammar/length.hpp>

#define SVGPP_PARSE_MISC_IMPL(IteratorType, CoordinateType) \
  template bool svgpp::detail::parse_viewBox<IteratorType, CoordinateType>( \
    IteratorType &, IteratorType, CoordinateType &, CoordinateType &, CoordinateType &, CoordinateType &); \
  template bool svgpp::detail::parse_bbox<IteratorType, CoordinateType>( \
    IteratorType &, IteratorType, CoordinateType &, CoordinateType &, CoordinateType &, CoordinateType &); \
  template bool svgpp::detail::parse_enable_background<IteratorType, svgpp::tag::source::attribute, CoordinateType>( \
    IteratorType &, IteratorType, svgpp::tag::source::attribute, CoordinateType &, CoordinateType &, CoordinateType &, CoordinateType &); \
  template bool svgpp::detail::parse_enable_background<IteratorType, svgpp::tag::source::css, CoordinateType>( \
    IteratorType &, IteratorType, svgpp::tag::source::css, CoordinateType &, CoordinateType &, CoordinateType &, CoordinateType &);

#define SVGPP_PARSE_CLIP_IMPL(IteratorType, LengthFactoryType) \
  template bool svgpp::detail::parse_clip<LengthFactoryType, IteratorType, svgpp::tag::source::css>( \
    LengthFactoryType const &, IteratorType &, IteratorType, svgpp::tag::source::css, \
    LengthFactoryType::length_type *); \
  template bool svgpp::detail::parse_clip<LengthFactoryType, IteratorType, svgpp::tag::source::attribute>( \
    LengthFactoryType const &, IteratorType &, IteratorType, svgpp::tag::source::attribute, \
    LengthFactoryType::length_type *); 

namespace svgpp { namespace detail 
{

template<class Iterator, class Coordinate>
bool parse_viewBox(Iterator & it, Iterator end, Coordinate & x, Coordinate & y, Coordinate & w, Coordinate & h)
{
  namespace qi = boost::spirit::qi;
  namespace phx = boost::phoenix;
  using qi::_1;

  SVGPP_STATIC_IF_SAFE const qi::real_parser<Coordinate, detail::svg_real_policies<Coordinate> > number;
  SVGPP_STATIC_IF_SAFE const detail::comma_wsp_rule_no_skip<Iterator> comma_wsp;
  return qi::parse(it, end,
    number[phx::ref(x) = _1] >> comma_wsp >>
    number[phx::ref(y) = _1] >> comma_wsp >>
    number[phx::ref(w) = _1] >> comma_wsp >>
    number[phx::ref(h) = _1]);
}

template<class Iterator, class Coordinate>
bool parse_bbox(Iterator & it, Iterator end, Coordinate & lo_x, Coordinate & lo_y, Coordinate & hi_x, Coordinate & hi_y)
{
  namespace qi = boost::spirit::qi;
  namespace phx = boost::phoenix;
  using qi::_1;

  SVGPP_STATIC_IF_SAFE const qi::real_parser<Coordinate, detail::svg_real_policies<Coordinate> > number;
  return qi::parse(it, end,
    number[phx::ref(lo_x) = _1] >> qi::lit(',') >>
    number[phx::ref(lo_y) = _1] >> qi::lit(',') >>
    number[phx::ref(hi_x) = _1] >> qi::lit(',') >>
    number[phx::ref(hi_y) = _1]);
}

template<class Iterator, class PropertySource, class Coordinate>
bool parse_enable_background(Iterator & it, Iterator end, 
  PropertySource property_source,
  Coordinate & x, Coordinate & y, Coordinate & width, Coordinate & height)
{
  namespace qi = boost::spirit::qi;
  namespace phx = boost::phoenix;
  using qi::_1;

  SVGPP_STATIC_IF_SAFE const qi::real_parser<Coordinate, detail::number_policies<Coordinate, PropertySource> > number;
  const qi::rule<Iterator> rule =
    detail::no_case_if_css(property_source)[qi::lit("new")]
    >> +detail::character_encoding_namespace::space
    >> number[phx::ref(x) = qi::_1]
    >> +detail::character_encoding_namespace::space
    >> number[phx::ref(y) = qi::_1]
    >> +detail::character_encoding_namespace::space
    >> number[phx::ref(width) = qi::_1]
    >> +detail::character_encoding_namespace::space
    >> number[phx::ref(height) = qi::_1];
  // TODO: check for negative values
  return qi::parse(it, end, rule);
}

template<class LengthFactory, class Iterator, class PropertySource>
bool parse_clip(
  LengthFactory const & length_factory,
  Iterator & it, Iterator end,
  PropertySource property_source,
  typename LengthFactory::length_type * out_rect)
{
  // 'auto' and 'inherit' values must be checked outside

  namespace qi = boost::spirit::qi;
  namespace phx = boost::phoenix;
  using qi::_1;

  SVGPP_STATIC_IF_SAFE const length_grammar<
    PropertySource,
    Iterator,
    LengthFactory,
    tag::length_dimension::width
  > length_grammar_x;
  SVGPP_STATIC_IF_SAFE const length_grammar<
    PropertySource,
    Iterator,
    LengthFactory,
    tag::length_dimension::height
  > length_grammar_y;

  // Initializing with values for 'auto'
  for (int i = 0; i<4; ++i)
    out_rect[i] = length_factory.create_length(0, tag::length_units::none());
  const qi::rule<Iterator> rule =
    detail::no_case_if_css(property_source)
    [
      qi::lit("rect") >> *detail::character_encoding_namespace::space >> qi::lit('(')
      >> *detail::character_encoding_namespace::space
    >> (qi::lit("auto")
      | length_grammar_y(phx::cref(length_factory))[phx::ref(out_rect[0]) = qi::_1]
      )
    >> +detail::character_encoding_namespace::space
    >> (qi::lit("auto")
      | length_grammar_x(phx::cref(length_factory))[phx::ref(out_rect[1]) = qi::_1]
      )
    >> +detail::character_encoding_namespace::space
    >> (qi::lit("auto")
      | length_grammar_y(phx::cref(length_factory))[phx::ref(out_rect[2]) = qi::_1]
      )
    >> +detail::character_encoding_namespace::space
    >> (qi::lit("auto")
      | length_grammar_x(phx::cref(length_factory))[phx::ref(out_rect[3]) = qi::_1]
      )
    >> *detail::character_encoding_namespace::space >> qi::lit(')')
    ];
  return qi::parse(it, end, rule);
}

}}