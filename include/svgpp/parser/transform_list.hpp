#pragma once

#include <svgpp/config.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/adapter/transform.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/grammar/transform.hpp>

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
      boost::parameter::optional<tag::transform_policy>,
      boost::parameter::optional<tag::load_transform_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef detail::bind_context_parameters_wrapper<Context, args2_t> context_t;
    typedef typename detail::unwrap_context<context_t, tag::transform_policy>::policy transform_policy;
    typedef typename detail::unwrap_context<context_t, tag::load_transform_policy> load_transform_context;
    typedef typename load_transform_context::policy load_transform_policy;
    typedef detail::transform_adapter_if_needed<context_t> adapted_context_t; 
    typedef transform_grammar<
      iterator_t, 
      typename adapted_context_t::type, 
      typename transform_policy::number_type,
      typename detail::unwrap_context<adapted_context_t::adapted_context, tag::load_transform_policy>::policy
    > transform_grammar_t;

    context_t bound_context(context);
    typename adapted_context_t::type transform_adapter(load_transform_context::get(bound_context));
    SVGPP_STATIC_IF_SAFE const transform_grammar_t grammar;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    if (boost::spirit::qi::phrase_parse(it, end, 
        grammar(boost::phoenix::ref(transform_adapter)), typename transform_grammar_t::skipper_type()) 
      && it == end)
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