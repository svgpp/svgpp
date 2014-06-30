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
    typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::transform_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2;
    typedef typename boost::parameter::value_type<args2, tag::transform_policy, 
      context_policy<tag::transform_policy, Context> >::type transform_policy;
    typedef detail::transform_adapter_if_needed<Context, transform_policy> adapted_context_t; 
    typedef transform_grammar<iterator_t, typename adapted_context_t::type, typename transform_policy::number_type> transform_grammar_t;

    typename adapted_context_t::holder_type adapted_context(context);
    SVGPP_STATIC_IF_SAFE const transform_grammar_t grammar;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    if (boost::spirit::qi::phrase_parse(it, end, 
        grammar(boost::phoenix::ref(adapted_context)), typename transform_grammar_t::skipper_type()) 
      && it == end)
    {
      adapted_context_t::on_exit_attribute(adapted_context);
      return true;
    }
    else
    {
      return args_t::get_error_policy::template apply<Context>::type::parse_failed(context, tag, attribute_value);
    }
  }
};

}