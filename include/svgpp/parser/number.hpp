// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/parser/detail/common.hpp>
#include <svgpp/parser/detail/finite_function_iterator.hpp>
#include <svgpp/parser/detail/parse_list_iterator.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace svgpp { 

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::number, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source)
  {
    namespace qi = boost::spirit::qi;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::number_type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;

    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    SVGPP_STATIC_IF_SAFE const qi::real_parser<coordinate_t, detail::number_policies<coordinate_t, PropertySource> > number;
    coordinate_t value;
    if (qi::parse(it, end, number, value) && it == end)
    {
      args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source, value);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::list_of<tag::type::number>, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source)
  {
    namespace qi = boost::spirit::qi;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::number_type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef qi::real_parser<coordinate_t, detail::number_policies<coordinate_t, PropertySource> > grammar_t;
    typedef detail::comma_wsp_rule_no_skip<iterator_t> separator_t;
    typedef detail::parse_list_iterator<coordinate_t, iterator_t, grammar_t, separator_t> parse_list_iterator_t;
    typedef detail::finite_function_iterator<parse_list_iterator_t> output_iterator_t;

    SVGPP_STATIC_IF_SAFE const grammar_t number_grammar;
    SVGPP_STATIC_IF_SAFE const separator_t separator_grammar;
    parse_list_iterator_t parse_list(
      boost::begin(attribute_value), boost::end(attribute_value), 
      number_grammar, separator_grammar);

    args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source,
      boost::make_iterator_range(output_iterator_t(parse_list), output_iterator_t()));
    if (parse_list.error())
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
    else
      return true;
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::number_optional_number, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::attribute property_source)
  {
    namespace qi = boost::spirit::qi;
    namespace phx = boost::phoenix;
    using qi::_1;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::number_type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;

    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    SVGPP_STATIC_IF_SAFE const qi::real_parser<coordinate_t, detail::number_policies<coordinate_t, tag::source::attribute> > number;
    SVGPP_STATIC_IF_SAFE const detail::comma_wsp_rule_no_skip<iterator_t> comma_wsp;
    coordinate_t value1, value2;
    bool two_values = false;
    if (qi::parse(it, end, 
          number [phx::ref(value1) = _1] 
          >> -(
              comma_wsp 
              >> number [phx::ref(value2) = _1, phx::ref(two_values) = true]
              )
      ) && it == end)
    {
      if (two_values)
        args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source, value1, value2);
      else
        args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source, value1);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

}
