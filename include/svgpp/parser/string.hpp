// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/policy/value_events.hpp>

namespace svgpp 
{

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::string, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source)
  {
    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source, attribute_value);
    return true;
  }
};

}