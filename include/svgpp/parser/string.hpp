#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/policy/load_value.hpp>

namespace svgpp 
{

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::string, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource)
  {
    policy::load_value::default_policy<Context>::set(context, tag, attribute_value);
    return true;
  }
};

}