// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <svgpp/config.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/detail/adapt_context.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/external_function/parse_length.hpp>
#if !defined(SVGPP_USE_EXTERNAL_LENGTH_PARSER)
# include <svgpp/parser/external_function/parse_length_impl.hpp>
# include <svgpp/parser/grammar/length.hpp>
#endif
#include <svgpp/parser/detail/common.hpp>
#include <svgpp/parser/detail/finite_function_iterator.hpp>
#include <svgpp/parser/detail/parse_list_iterator.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/traits/length_dimension_by_attribute.hpp>

namespace svgpp 
{ 

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::length, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource source)
  {
    typedef typename traits::length_dimension_by_attribute<AttributeTag>::type direction_t;
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
    if (detail::parse_length<direction_t, PropertySource>(length_factory, it, end, value)
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

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::list_of<tag::type::length>, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source)
  {
    typedef typename traits::length_dimension_by_attribute<AttributeTag>::type direction_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    return parseT<
      detail::length_grammar_tag,
      AttributeTag, Context, AttributeValue, PropertySource, direction_t>
      (tag, context, attribute_value, property_source);
  }

protected:
  template<class LengthGrammarTag, class AttributeTag, class Context, class AttributeValue, class PropertySource, class Direction>
  static bool parseT(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source)
  {
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::length_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename detail::unwrap_context<Context, tag::length_policy> length_policy_context;
    typedef typename length_policy_context::template bind<args2_t>::type length_policy_t;
    typedef typename length_policy_t::length_factory_type length_factory_t;
    typedef qi::rule<iterator_t, typename length_factory_t::length_type()> length_rule_t;
    typedef detail::comma_wsp_rule_no_skip<iterator_t> separator_t;
    typedef detail::parse_list_iterator<
      typename length_factory_t::length_type,
      iterator_t, 
      length_rule_t, 
      separator_t
    > parse_list_iterator_t;
    typedef detail::finite_function_iterator<parse_list_iterator_t> output_iterator_t;

    length_factory_t & length_factory = length_policy_t::length_factory(length_policy_context::get(context));
    length_rule_t length_rule;
#ifdef SVGPP_USE_EXTERNAL_LENGTH_PARSER
    length_rule %= detail::get_length_rule<
      Direction,
      iterator_t,
      PropertySource
    >(LengthGrammarTag(), length_factory)(boost::phoenix::ref(length_factory));
#else
    SVGPP_STATIC_IF_SAFE const typename boost::mpl::if_<
      boost::is_same<LengthGrammarTag, detail::length_grammar_tag>,
      length_grammar<
        PropertySource, 
        iterator_t, 
        length_factory_t, 
        Direction,
        typename length_factory_t::number_type
      >,
      percentage_or_length_css_grammar<
        iterator_t, 
        length_factory_t, 
        typename length_factory_t::number_type
      >
    >::type length_grammar;
    length_rule %= length_grammar(boost::phoenix::ref(length_factory));
#endif

    SVGPP_STATIC_IF_SAFE const separator_t separator_grammar;
    parse_list_iterator_t parse_list(
      boost::begin(attribute_value), boost::end(attribute_value), 
      length_rule,
      separator_grammar);

    args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source,
      boost::make_iterator_range(output_iterator_t(parse_list), output_iterator_t()));
    if (parse_list.error())
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
    else
      return true;
  }
};

}