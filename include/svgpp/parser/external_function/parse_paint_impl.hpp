// Copyright Oleg Maximenko 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/parser/external_function/parse_paint.hpp>
#include <svgpp/parser/grammar/color_optional_icc_color.hpp>
#include <svgpp/parser/grammar/iri.hpp>

#define SVGPP_PARSE_PAINT_IMPL(IteratorType, ColorFactory, IccColorFactory) \
  template bool svgpp::detail::parse_paint<ColorFactory, IccColorFactory, IteratorType, svgpp::tag::source::attribute>( \
    IccColorFactory const &, IteratorType &, IteratorType, svgpp::tag::source::attribute, \
    svgpp::detail::paint_option &, svgpp::detail::paint_option &, \
    boost::tuple<ColorFactory::color_type, boost::optional<IccColorFactory::icc_color_type> > &, \
    boost::iterator_range<IteratorType> &); \
  template bool svgpp::detail::parse_paint<ColorFactory, IccColorFactory, IteratorType, svgpp::tag::source::css>( \
    IccColorFactory const &, IteratorType &, IteratorType, svgpp::tag::source::css, \
    svgpp::detail::paint_option &, svgpp::detail::paint_option &, \
    boost::tuple<ColorFactory::color_type, boost::optional<IccColorFactory::icc_color_type> > &, \
    boost::iterator_range<IteratorType> &);

namespace svgpp { namespace detail 
{

template<class ColorFactory, class IccColorFactory, class Iterator, class PropertySource>
bool parse_paint(
  IccColorFactory const & icc_color_factory,
  Iterator & it, Iterator end, PropertySource property_source,
  paint_option & out_main_option, paint_option & out_funciri_suboption,
  boost::tuple<
  typename ColorFactory::color_type, boost::optional<typename IccColorFactory::icc_color_type>
  > & out_color,
  typename boost::iterator_range<Iterator> & out_iri)
{
  namespace qi = boost::spirit::qi;
  namespace phx = boost::phoenix;
  using detail::character_encoding_namespace::space;

  SVGPP_STATIC_IF_SAFE const color_optional_icc_color_grammar<
    PropertySource, Iterator, ColorFactory, IccColorFactory> color_optional_icc_color;
  SVGPP_STATIC_IF_SAFE const funciri_grammar<PropertySource, Iterator> funciri_rule;

  qi::rule<Iterator> rule = 
    detail::no_case_if_css(property_source) [
        qi::lit("none")           [phx::ref(out_main_option) = paint_option_none]
      | qi::lit("currentColor")   [phx::ref(out_main_option) = paint_option_currentColor]
      | qi::lit("inherit")        [phx::ref(out_main_option) = paint_option_inherit]
      | color_optional_icc_color(boost::phoenix::ref(icc_color_factory))  
          [phx::ref(out_color) = qi::_1, phx::ref(out_main_option) = paint_option_color]
      | ( funciri_rule [phx::ref(out_iri) = qi::_1]
          >> - ( +space
                  >> ( qi::lit("none")          [phx::ref(out_funciri_suboption) = paint_option_none]
                      | qi::lit("currentColor")  [phx::ref(out_funciri_suboption) = paint_option_currentColor]
                      | color_optional_icc_color(boost::phoenix::ref(icc_color_factory)) 
                        [phx::ref(out_color) = qi::_1, phx::ref(out_funciri_suboption) = paint_option_color]
                      )
                )
        ) [phx::ref(out_main_option) = paint_option_funciri]
    ];

  return qi::parse(it, end, rule);
}

}}