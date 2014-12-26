// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/grammar/clock_value.hpp>
#include <boost/spirit/include/qi.hpp>

namespace svgpp
{

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::clock_value, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::attribute source)
  {
    namespace qi = boost::spirit::qi;

    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;

    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    // TODO: own type for clock_value
    SVGPP_STATIC_IF_SAFE const clock_value_grammar<iterator_t, typename args_t::number_type> grammar;
    typename args_t::number_type value;
    if (qi::parse(it, end, grammar, value) && it == end)
    {
      args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, source, value);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
    return true;
  }
};

}
