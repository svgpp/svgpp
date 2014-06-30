#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/parser/simple_enumeration.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>

namespace svgpp
{

template<class InnerType, class Value,
    BOOST_PP_ENUM_PARAMS(SVGPP_TYPE_OR_LITERAL_ARITY, class Value),
    SVGPP_TEMPLATE_ARGS>
struct value_parser<
  tag::type::type_or_literal<InnerType, Value, BOOST_PP_ENUM_PARAMS(SVGPP_TYPE_OR_LITERAL_ARITY, Value)>, SVGPP_TEMPLATE_ARGS_PASS>
  : value_parser<InnerType, SVGPP_TEMPLATE_ARGS_PASS>
{
  typedef value_parser<InnerType, SVGPP_TEMPLATE_ARGS_PASS> inner_parser;

  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, 
    AttributeValue const & attribute_value, PropertySource property_source)
  {
    typedef detail::literal_values_dictionary<typename boost::range_value<AttributeValue>::type> dictionary;
    typedef boost::mpl::vector<Value, BOOST_PP_ENUM_PARAMS(SVGPP_TYPE_OR_LITERAL_ARITY, Value)> tag_list;

    detail::simple_enumeration_type_visitor<
      dictionary, AttributeTag, Context, AttributeValue,
      boost::is_same<PropertySource, tag::source::attribute>::value
    > fn(
      context, attribute_value);
    boost::mpl::for_each<tag_list>(boost::ref(fn));
    if (!fn.found())
      return inner_parser::parse(tag, context, attribute_value, property_source);
    else
      return true;
  }

  template<class AttributeTag, class Context, class AttributeValue, class PropertySource, class LengthFactory>
  static bool parse(AttributeTag tag, Context & context, 
    AttributeValue const & attribute_value, PropertySource property_source,
    LengthFactory const & length_factory)
  {
    typedef detail::literal_values_dictionary<typename boost::range_value<AttributeValue>::type> dictionary;
    typedef boost::mpl::vector<Value, BOOST_PP_ENUM_PARAMS(SVGPP_TYPE_OR_LITERAL_ARITY, Value)> tag_list;

    detail::simple_enumeration_type_visitor<
      dictionary, AttributeTag, Context, AttributeValue,
      boost::is_same<PropertySource, tag::source::attribute>::value
    > fn(
      context, attribute_value);
    boost::mpl::for_each<tag_list>(boost::ref(fn));
    if (!fn.found())
      return inner_parser::parse(tag, context, attribute_value, property_source, length_factory);
    else
      return true;
  }
};

}