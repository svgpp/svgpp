// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/parser/literal_enumeration.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>

namespace svgpp
{

template<class InnerType, class Value,
    BOOST_PP_ENUM_PARAMS(SVGPP_TYPE_OR_LITERAL_ARITY, class Value),
    SVGPP_TEMPLATE_ARGS>
struct value_parser<
  tag::type::type_or_literal<InnerType, Value, BOOST_PP_ENUM_PARAMS(SVGPP_TYPE_OR_LITERAL_ARITY, Value)>, 
  SVGPP_TEMPLATE_ARGS_PASS
>
{
  typedef value_parser<InnerType, SVGPP_TEMPLATE_ARGS_PASS> inner_parser;

  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, 
    AttributeValue const & attribute_value, PropertySource property_source)
  {
    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef detail::literal_values_dictionary<typename boost::range_value<AttributeValue>::type> dictionary;
    typedef boost::mpl::vector<Value, BOOST_PP_ENUM_PARAMS(SVGPP_TYPE_OR_LITERAL_ARITY, Value)> tag_list;

    detail::literal_enumeration_type_visitor<
      dictionary, 
      AttributeTag, 
      typename args_t::value_events_context::type, 
      typename args_t::value_events_policy,
      AttributeValue,
      PropertySource
    > fn(args_t::value_events_context::get(context), attribute_value);
    boost::mpl::for_each<tag_list>(boost::ref(fn));
    if (!fn.found())
      return inner_parser::parse(tag, context, attribute_value, property_source);
    else
      return true;
  }
};

}