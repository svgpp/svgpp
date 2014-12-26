// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/parser/detail/finite_function_iterator.hpp>
#include <svgpp/parser/detail/parse_list_iterator.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/grammar/coordinate_pair.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>

namespace svgpp 
{

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::attribute::points, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class Context, class AttributeValue>
  static bool parse(tag::attribute::points tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::attribute property_source)
  {
    namespace qi = boost::spirit::qi;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef typename args_t::number_type coordinate_t;
    typedef coordinate_pair_grammar<iterator_t, coordinate_t> pair_grammar_t;
    typedef detail::comma_wsp_rule_no_skip<iterator_t> separator_t;
    typedef detail::parse_list_iterator<
      std::pair<coordinate_t, coordinate_t>,
      iterator_t, 
      pair_grammar_t, 
      separator_t,
      detail::character_encoding_namespace::space_type
    > parse_list_iterator_t;
    typedef detail::finite_function_iterator<parse_list_iterator_t> output_iterator_t;

    SVGPP_STATIC_IF_SAFE const pair_grammar_t pair_grammar;
    SVGPP_STATIC_IF_SAFE const separator_t separator_grammar;
    parse_list_iterator_t parse_list(
      boost::begin(attribute_value), boost::end(attribute_value), 
      pair_grammar, separator_grammar, detail::character_encoding_namespace::space);

    args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source,
      boost::make_iterator_range(output_iterator_t(parse_list), output_iterator_t()));
    if (parse_list.error())
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    else
      return true;
  }
};

}