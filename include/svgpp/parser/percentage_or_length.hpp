// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/parser/length.hpp>

namespace svgpp 
{ 

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::percentage_or_length, SVGPP_TEMPLATE_ARGS_PASS>
  : value_parser<tag::type::length, SVGPP_TEMPLATE_ARGS_PASS>
{
  using value_parser<tag::type::length, SVGPP_TEMPLATE_ARGS_PASS>::parse;

  template<class AttributeTag, class Context, class AttributeValue>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::css property_source)
  {
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::length_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename detail::unwrap_context<Context, tag::length_policy> length_policy_context;
    typedef typename length_policy_context::template bind<args2_t>::type length_policy_t;

    typename length_policy_t::length_factory_type & length_factory 
      = length_policy_t::length_factory(length_policy_context::get(context));
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    typename length_policy_t::length_factory_type::length_type value;
    if (detail::parse_percentage_or_length(length_factory, it, end, value)
      && it == end)
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
struct value_parser<tag::type::list_of<tag::type::percentage_or_length>, SVGPP_TEMPLATE_ARGS_PASS>
  : value_parser<tag::type::list_of<tag::type::length>, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef value_parser<tag::type::list_of<tag::type::length>, SVGPP_TEMPLATE_ARGS_PASS> base_type;

  using base_type::parse;

  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::css property_source)
  {
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    return base_type::template parseT<
      detail::percentage_or_length_grammar_tag,
      AttributeTag, Context, AttributeValue, 
      tag::source::css, 
      tag::length_dimension::not_width_nor_height>
      (tag, context, attribute_value, property_source);
  }
};

}