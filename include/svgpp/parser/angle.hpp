// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/grammar/angle.hpp>

namespace svgpp
{

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::angle, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource source)
  {
    namespace qi = boost::spirit::qi;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::angle_factory>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename detail::unwrap_context<Context, tag::angle_factory>::template bind<args2_t>::type angle_factory_t;

    SVGPP_STATIC_IF_SAFE const angle_grammar<PropertySource, iterator_t, angle_factory_t, typename args_t::number_type> grammar;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    typename angle_factory_t::angle_type value;
    if (boost::spirit::qi::parse(it, end, grammar, value) 
      && it == end)
    {
      args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, source, value);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

}