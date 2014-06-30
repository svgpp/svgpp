#pragma once

#include <svgpp/parser/length.hpp>

namespace svgpp 
{ 

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::percentage_or_length, SVGPP_TEMPLATE_ARGS_PASS>
  : value_parser<tag::type::length, SVGPP_TEMPLATE_ARGS_PASS>
{
  struct requires_length_factory;

  using value_parser<tag::type::length, SVGPP_TEMPLATE_ARGS_PASS>::parse;

  template<class AttributeTag, class Context, class AttributeValue, class LengthFactory>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::css,
                                    LengthFactory const & length_factory)
  {
    typedef typename traits::length_direction_by_attribute<AttributeTag>::type direction_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;

    SVGPP_STATIC_IF_SAFE const percentage_or_length_css_grammar<iterator_t, LengthFactory, direction_t> length_grammar;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    typename LengthFactory::length_type value;
    if (boost::spirit::qi::parse(it, end, length_grammar(boost::phoenix::ref(length_factory)), value) 
      && it == end)
    {
      context_policy<tag::load_value_policy, Context>::set(context, tag, value);
      return true;
    }
    else
    {
      typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
      return args_t::get_error_policy::template apply<Context>::type::parse_failed(context, tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::list_of<tag::type::percentage_or_length>, SVGPP_TEMPLATE_ARGS_PASS>
  : value_parser<tag::type::list_of<tag::type::length>, SVGPP_TEMPLATE_ARGS_PASS>
{
  using value_parser<tag::type::list_of<tag::type::length>, SVGPP_TEMPLATE_ARGS_PASS>::parse;

  template<class AttributeTag, class Context, class AttributeValue, class PropertySource, class LengthFactory>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::css property_source,
                                    LengthFactory const & length_factory)
  {
    typedef typename traits::length_direction_by_attribute<AttributeTag>::type direction_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef percentage_or_length_css_grammar<iterator_t, LengthFactory, direction_t> grammar_t;
    return parseT<grammar_t, AttributeTag, Context, AttributeValue, tag::source::css, LengthFactory>
      (tag, context, attribute_value, property_source, length_factory);
  }
};

}