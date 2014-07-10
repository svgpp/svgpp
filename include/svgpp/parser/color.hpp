#pragma once

#include <svgpp/factory/color.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/grammar/color.hpp>
#include <svgpp/parser/grammar/color_optional_icc_color.hpp>
#include <svgpp/policy/load_value.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(color_factory)
BOOST_PARAMETER_TEMPLATE_KEYWORD(icc_color_factory)

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::color, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource)
  {
    namespace qi = boost::spirit::qi;

    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::color_factory>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename boost::parameter::value_type<args2_t, tag::color_factory, 
      typename factory::color::by_context<Context>::type>::type color_factory_t;

    SVGPP_STATIC_IF_SAFE const color_grammar<iterator_t, color_factory_t> color_rule;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    typename color_factory_t::color_type color;
    if (qi::parse(it, end, color_rule, color) && it == end)
    {
      policy::load_value::default_policy<Context>::set(context, tag, color);
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
struct value_parser<tag::type::color_optional_icc_color, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource)
  {
    namespace qi = boost::spirit::qi;

    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::color_factory>,
      boost::parameter::optional<tag::icc_color_factory>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename boost::parameter::value_type<args2_t, tag::color_factory, 
      typename factory::color::by_context<Context>::type>::type color_factory_t;
    typedef typename boost::parameter::value_type<args2_t, tag::icc_color_factory, 
      typename factory::icc_color::by_context<Context>::type>::type icc_color_factory_t;

    SVGPP_STATIC_IF_SAFE const color_optional_icc_color_grammar<
      PropertySource, iterator_t, color_factory_t, icc_color_factory_t> color_rule;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    boost::tuple<typename color_factory_t::color_type, boost::optional<typename icc_color_factory_t::icc_color_type> > color;
    if (qi::parse(it, end, color_rule, color) && it == end)
    {
      if (color.get<1>())
        policy::load_value::default_policy<Context>::set(context, tag, color.get<0>(), *color.get<1>());
      else
        policy::load_value::default_policy<Context>::set(context, tag, color.get<0>());
      return true;
    }
    else
    {
      typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
      return args_t::template get_error_policy<Context>::type::parse_failed(context, tag, attribute_value);
    }
  }
};

}