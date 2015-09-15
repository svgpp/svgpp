// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/adapter/transform.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#if defined(SVGPP_USE_EXTERNAL_TRANSFORM_PARSER)
# include <svgpp/parser/external_function/parse_transform.hpp>
# include <svgpp/parser/external_function/transform_events_interface_proxy.hpp>
#else
# include <svgpp/parser/grammar/transform.hpp>
#endif

namespace svgpp
{

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::transform_list, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::attribute)
  {
    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::transform_events_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef detail::bind_context_parameters_wrapper<Context, args2_t> context_t;
    typedef typename detail::unwrap_context<context_t, tag::transform_events_policy> transform_events_context;
    typedef typename transform_events_context::policy transform_events_policy;
    typedef detail::transform_adapter_if_needed<context_t> adapted_context_t; 
    typedef 
      typename detail::unwrap_context<typename adapted_context_t::adapted_context, tag::transform_events_policy>::policy
        adapted_transform_events_policy;

    context_t bound_context(context);
    typename adapted_context_t::type transform_adapter(transform_events_context::get(bound_context));
    typename adapted_context_t::adapted_context_holder adapted_transform_context(adapted_context_t::adapt_context(bound_context, transform_adapter));
    
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
#if defined(SVGPP_USE_EXTERNAL_TRANSFORM_PARSER)
    detail::transform_events_interface_proxy<
      typename adapted_transform_events_policy::context_type,
      typename args_t::number_type,
      adapted_transform_events_policy
    > events_interface_proxy(
      detail::unwrap_context<typename adapted_context_t::adapted_context, tag::transform_events_policy>::get(
        adapted_transform_context));
    if (detail::parse_transform<iterator_t, typename args_t::number_type>(it, end, events_interface_proxy)
      && it == end)
#else
    typedef transform_grammar<
      iterator_t, 
      typename adapted_transform_events_policy::context_type,
      typename args_t::number_type,
      adapted_transform_events_policy
    > transform_grammar_t;
    SVGPP_STATIC_IF_SAFE const transform_grammar_t grammar;
    if (boost::spirit::qi::phrase_parse(it, end, 
        grammar(boost::phoenix::ref(
          detail::unwrap_context<typename adapted_context_t::adapted_context, tag::transform_events_policy>::get(
            adapted_transform_context))), 
        typename transform_grammar_t::skipper_type()) 
      && it == end)
#endif
    {
      adapted_context_t::on_exit_attribute(transform_adapter);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

}