#pragma once

#include <svgpp/config.hpp>
#include <svgpp/context_policy_iri.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/detail/pass_iri_value.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/grammar/iri.hpp>

namespace svgpp
{

namespace detail
{

template<class GetGrammarMetafunction, SVGPP_TEMPLATE_ARGS>
struct iri_value_parser
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, 
    AttributeValue const & attribute_value, PropertySource property_source)
  {
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::iri_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename boost::parameter::value_type<args2_t, tag::iri_policy, 
      context_policy<tag::iri_policy, Context> >::type iri_policy_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;

    SVGPP_STATIC_IF_SAFE GetGrammarMetafunction::apply<iterator_t>::type iri_rule;
    boost::iterator_range<iterator_t> iri;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    if (qi::parse(it, end, iri_rule, iri) && it == end)
    {
      typedef load_value_with_iri_policy<
        context_policy<tag::load_value_policy, Context>, 
        iri_policy_t
      >::type load_value_policy_t;
      load_value_policy_t::set(context, tag, iri);
      return true;
    }
    else
    {
      typedef value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
      return args_t::get_error_policy::template apply<Context>::type::parse_failed(context, tag, attribute_value);
    }
  }
};

} // namespace detail

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::iri, SVGPP_TEMPLATE_ARGS_PASS>
  : detail::iri_value_parser<boost::mpl::quote1<iri_grammar>, SVGPP_TEMPLATE_ARGS_PASS>
{
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::funciri, SVGPP_TEMPLATE_ARGS_PASS>
  : detail::iri_value_parser<boost::mpl::quote1<funciri_grammar>, SVGPP_TEMPLATE_ARGS_PASS>
{
};

}