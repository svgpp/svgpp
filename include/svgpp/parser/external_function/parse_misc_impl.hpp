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

#define SVGPP_PARSE_MISC_IMPL(IteratorType, CoordinateType) \
  template bool svgpp::detail::parse_viewBox<IteratorType, CoordinateType>( \
    IteratorType &, IteratorType, CoordinateType &, CoordinateType &, CoordinateType &, CoordinateType &); \
  template bool svgpp::detail::parse_bbox<IteratorType, CoordinateType>( \
    IteratorType &, IteratorType, CoordinateType &, CoordinateType &, CoordinateType &, CoordinateType &); \
  template bool svgpp::detail::parse_enable_background<IteratorType, CoordinateType, svgpp::tag::source::attribute>( \
    IteratorType &, IteratorType, CoordinateType &, CoordinateType &, CoordinateType &, CoordinateType &, svgpp::tag::source::attribute); \
  template bool svgpp::detail::parse_enable_background<IteratorType, CoordinateType, svgpp::tag::source::css>( \
    IteratorType &, IteratorType, CoordinateType &, CoordinateType &, CoordinateType &, CoordinateType &, svgpp::tag::source::css);

namespace svgpp { namespace detail 
{

template<class Iterator, class Coordinate>
bool parse_viewBox(Iterator & it, Iterator end, Coordinate & x, Coordinate & y, Coordinate & w, Coordinate & h)
{
  namespace qi = boost::spirit::qi;
  using namespace boost::phoenix;
  using qi::_1;

  SVGPP_STATIC_IF_SAFE const qi::real_parser<Coordinate, detail::svg_real_policies<Coordinate> > number;
  SVGPP_STATIC_IF_SAFE const detail::comma_wsp_rule_no_skip<Iterator> comma_wsp;
  return qi::parse(it, end,
    number[ref(x) = _1] >> comma_wsp >>
    number[ref(y) = _1] >> comma_wsp >>
    number[ref(w) = _1] >> comma_wsp >>
    number[ref(h) = _1]);
}

template<class Iterator, class Coordinate>
bool parse_bbox(Iterator & it, Iterator end, Coordinate & lo_x, Coordinate & lo_y, Coordinate & hi_x, Coordinate & hi_y)
{
  namespace qi = boost::spirit::qi;
  using namespace boost::phoenix;
  using qi::_1;

  SVGPP_STATIC_IF_SAFE const qi::real_parser<Coordinate, detail::svg_real_policies<Coordinate> > number;
  return qi::parse(it, end,
    number[ref(lo_x) = _1] >> qi::lit(',') >>
    number[ref(lo_y) = _1] >> qi::lit(',') >>
    number[ref(hi_x) = _1] >> qi::lit(',') >>
    number[ref(hi_y) = _1]);
}

template<class Iterator, class Coordinate, class PropertySource>
bool parse_enable_background(Iterator & it, Iterator end, 
  Coordinate & x, Coordinate & y, Coordinate & width, Coordinate & height,
  PropertySource property_source)
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

}}