#pragma once

#include <svgpp/config.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/grammar/length.hpp>
#include <svgpp/parser/detail/common.hpp>
#include <svgpp/parser/detail/finite_function_iterator.hpp>
#include <svgpp/parser/detail/parse_list_iterator.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/traits/length_direction_by_attribute.hpp>

namespace svgpp 
{ 

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::length, SVGPP_TEMPLATE_ARGS_PASS>
{
  struct requires_length_factory;

  template<class AttributeTag, class Context, class AttributeValue, class PropertySource, class LengthFactory>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource,
                                    LengthFactory const & length_factory)
  {
    typedef typename traits::length_direction_by_attribute<AttributeTag>::type direction_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;

    SVGPP_STATIC_IF_SAFE const length_grammar<PropertySource, iterator_t, LengthFactory, direction_t> length_grammar;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    typename LengthFactory::length_type value;
    if (boost::spirit::qi::parse(it, end, length_grammar(boost::phoenix::ref(length_factory)), value) 
      && it == end)
    {
      policy::load_value::default_policy<Context>::set(context, tag, value);
      return true;
    }
    else
    {
      typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
      return args_t::template get_error_policy<Context>::type::parse_failed(context, tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::list_of<tag::type::length>, SVGPP_TEMPLATE_ARGS_PASS>
{
  struct requires_length_factory;

  template<class AttributeTag, class Context, class AttributeValue, class PropertySource, class LengthFactory>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source,
                                    LengthFactory const & length_factory)
  {
    typedef typename traits::length_direction_by_attribute<AttributeTag>::type direction_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef length_grammar<PropertySource, iterator_t, LengthFactory, direction_t> grammar_t;
    return parseT<grammar_t, AttributeTag, Context, AttributeValue, PropertySource, LengthFactory>
      (tag, context, attribute_value, property_source, length_factory);
  }

protected:
  template<class LengthGrammar, class AttributeTag, class Context, class AttributeValue, class PropertySource, class LengthFactory>
  static bool parseT(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source,
                                    LengthFactory const & length_factory)
  {
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef qi::rule<iterator_t, typename LengthFactory::length_type()> length_rule_t;
    typedef detail::comma_wsp_rule_no_skip<iterator_t> separator_t;
    typedef detail::parse_list_iterator<iterator_t, length_rule_t, separator_t, typename LengthFactory::length_type> parse_list_iterator_t;
    typedef detail::finite_function_iterator<parse_list_iterator_t> output_iterator_t;

    SVGPP_STATIC_IF_SAFE const LengthGrammar length_grammar;
    length_rule_t length_rule;
    length_rule %= length_grammar(boost::phoenix::ref(length_factory));
    SVGPP_STATIC_IF_SAFE const separator_t separator_grammar;
    parse_list_iterator_t parse_list(
      boost::begin(attribute_value), boost::end(attribute_value), 
      length_rule,
      separator_grammar);

    policy::load_value::default_policy<Context>::set(context, tag, 
      boost::make_iterator_range(output_iterator_t(parse_list), output_iterator_t()));
    if (parse_list.error())
    {
      typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
      return args_t::template get_error_policy<Context>::type::parse_failed(context, tag, attribute_value);
    }
    else
      return true;
  }
};

}