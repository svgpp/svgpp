// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
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
    typedef value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::iri_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename detail::unwrap_context<Context, tag::iri_policy>::template bind<args2_t>::type iri_policy_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;

    SVGPP_STATIC_IF_SAFE typename GetGrammarMetafunction::template apply<PropertySource, iterator_t>::type iri_rule;
    boost::iterator_range<iterator_t> iri;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    if (qi::parse(it, end, iri_rule, iri) && it == end)
    {
      typedef typename value_events_with_iri_policy<
        typename args_t::value_events_policy, 
        iri_policy_t
      >::type value_events_policy_t;
      value_events_policy_t::set(args_t::value_events_context::get(context), tag, property_source, iri);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

} // namespace detail

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::iri, SVGPP_TEMPLATE_ARGS_PASS>
  : detail::iri_value_parser<boost::mpl::bind1<boost::mpl::quote1<iri_grammar>, boost::mpl::_2>, SVGPP_TEMPLATE_ARGS_PASS>
{
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::funciri, SVGPP_TEMPLATE_ARGS_PASS>
  : detail::iri_value_parser<boost::mpl::quote2<funciri_grammar>, SVGPP_TEMPLATE_ARGS_PASS>
{
};

}