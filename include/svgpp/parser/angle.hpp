#pragma once

#include <svgpp/config.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/factory/angle.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/grammar/angle.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(angle_factory)

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::angle, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source)
  {
    namespace qi = boost::spirit::qi;

    typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::template get_number_type<Context>::type number_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::angle_factory>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename boost::parameter::value_type<args2_t, tag::angle_factory,
      typename factory::angle::by_context<Context>::type>::type angle_factory_t;

    SVGPP_STATIC_IF_SAFE const angle_grammar<PropertySource, iterator_t, angle_factory_t, number_t> grammar;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    typename angle_factory_t::angle_type value;
    if (boost::spirit::qi::parse(it, end, grammar, value) 
      && it == end)
    {
      policy::load_value::default_policy<Context>::set(context, tag, value);
      return true;
    }
    else
    {
      return args_t::template get_error_policy<Context>::type::parse_failed(context, tag, attribute_value);
    }
  }
};

}