// Copyright Oleg Maximenko 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/parser/external_function/parse_color.hpp>
#include <svgpp/parser/grammar/color.hpp>
#include <svgpp/parser/grammar/color_optional_icc_color.hpp>

#define SVGPP_PARSE_COLOR_IMPL(IteratorType, ColorFactory, IccColorFactory) \
  template bool svgpp::detail::parse_color<ColorFactory, IteratorType, svgpp::tag::source::attribute>( \
    IteratorType &, IteratorType, svgpp::tag::source::attribute, \
    ColorFactory::color_type &); \
  template bool svgpp::detail::parse_color<ColorFactory, IteratorType, svgpp::tag::source::css>( \
    IteratorType &, IteratorType, svgpp::tag::source::css, \
    ColorFactory::color_type &); \
  template bool svgpp::detail::parse_color_optional_icc_color<ColorFactory, IccColorFactory, IteratorType, svgpp::tag::source::attribute>( \
    IccColorFactory const &, IteratorType &, IteratorType, svgpp::tag::source::attribute, \
    boost::tuple<ColorFactory::color_type, boost::optional<IccColorFactory::icc_color_type> > &); \
  template bool svgpp::detail::parse_color_optional_icc_color<ColorFactory, IccColorFactory, IteratorType, svgpp::tag::source::css>( \
    IccColorFactory const &, IteratorType &, IteratorType, svgpp::tag::source::css, \
    boost::tuple<ColorFactory::color_type, boost::optional<IccColorFactory::icc_color_type> > &);

namespace svgpp { namespace detail 
{

template<class ColorFactory, class Iterator, class PropertySource>
bool parse_color(
  Iterator & it, Iterator end, PropertySource property_source,
  typename ColorFactory::color_type & out_color)
{
  namespace qi = boost::spirit::qi;

  SVGPP_STATIC_IF_SAFE const color_grammar<PropertySource, Iterator, ColorFactory> color_rule;
  return qi::parse(it, end, color_rule, out_color);
}

template<class ColorFactory, class IccColorFactory, class Iterator, class PropertySource>
bool parse_color_optional_icc_color(
  IccColorFactory const & icc_color_factory,
  Iterator & it, Iterator end, PropertySource property_source,
  boost::tuple<
    typename ColorFactory::color_type, boost::optional<typename IccColorFactory::icc_color_type>
  > & out_color)
{
  namespace qi = boost::spirit::qi;

  SVGPP_STATIC_IF_SAFE const color_optional_icc_color_grammar<
    PropertySource, Iterator, ColorFactory, IccColorFactory> color_rule;
  return qi::parse(it, end, color_rule(boost::phoenix::ref(icc_color_factory)), out_color);
}

}}