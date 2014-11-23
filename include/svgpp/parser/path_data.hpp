// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/adapter/path_markers.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/grammar/path_data.hpp>

namespace svgpp
{

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::path_data, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::attribute)
  {
    namespace qi = boost::spirit::qi;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::number_type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;

    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::path_events_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef detail::bind_context_parameters_wrapper<Context, args2_t> context_t;
    typedef typename detail::unwrap_context<context_t, tag::path_events_policy> path_events_context;
    typedef detail::path_adapter_if_needed<context_t> adapted_context_t; 
    typedef path_data_grammar<
      iterator_t, 
      typename detail::unwrap_context<typename adapted_context_t::adapted_context, tag::path_events_policy>::type,
      coordinate_t,
      typename detail::unwrap_context<typename adapted_context_t::adapted_context, tag::path_events_policy>::policy
    > path_data_grammar;

    context_t bound_context(context);
    typename adapted_context_t::type path_adapter(path_events_context::get(bound_context));
    typename adapted_context_t::adapted_context_holder adapted_path_context(adapted_context_t::adapt_context(bound_context, path_adapter));
    SVGPP_STATIC_IF_SAFE const path_data_grammar grammar;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    if (qi::phrase_parse(it, end, grammar(boost::phoenix::ref(
        detail::unwrap_context<typename adapted_context_t::adapted_context, tag::path_events_policy>::get(
          adapted_path_context))), 
        typename path_data_grammar::skipper_type()) 
      && it == end)
    {
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

}