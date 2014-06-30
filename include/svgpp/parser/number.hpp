#pragma once

#include <svgpp/config.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/parser/detail/common.hpp>
#include <svgpp/parser/detail/finite_function_iterator.hpp>
#include <svgpp/parser/detail/parse_list_iterator.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace svgpp { 

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::number, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource)
  {
    namespace qi = boost::spirit::qi;

    typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::get_number_type::template apply<Context>::type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;

    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    SVGPP_STATIC_IF_SAFE const qi::real_parser<coordinate_t, detail::number_policies<coordinate_t, PropertySource> > number;
    coordinate_t value;
    if (qi::parse(it, end, number, value) && it == end)
    {
      context_policy<tag::load_value_policy, Context>::set(context, tag, value);
      return true;
    }
    else
    {
      return args_t::get_error_policy::template apply<Context>::type::parse_failed(context, tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::list_of<tag::type::number>, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource)
  {
    namespace qi = boost::spirit::qi;

    typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::get_number_type::template apply<Context>::type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef qi::real_parser<coordinate_t, detail::number_policies<coordinate_t, PropertySource> > grammar_t;
    typedef detail::comma_wsp_rule_no_skip<iterator_t> separator_t;
    typedef detail::parse_list_iterator<iterator_t, grammar_t, separator_t, coordinate_t> parse_list_iterator_t;
    typedef detail::finite_function_iterator<parse_list_iterator_t> output_iterator_t;

    SVGPP_STATIC_IF_SAFE const grammar_t number_grammar;
    SVGPP_STATIC_IF_SAFE const separator_t separator_grammar;
    parse_list_iterator_t parse_list(
      boost::begin(attribute_value), boost::end(attribute_value), 
      number_grammar, separator_grammar);

    context_policy<tag::load_value_policy, Context>::set(context, tag, 
      boost::make_iterator_range(output_iterator_t(parse_list), output_iterator_t()));
    if (parse_list.error())
    {
      typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
      return args_t::get_error_policy::template apply<Context>::type::parse_failed(context, tag, attribute_value);
    }
    else
      return true;
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::number_optional_number, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::attribute)
  {
    namespace qi = boost::spirit::qi;
    using namespace boost::phoenix;
    using qi::_1;

    typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::get_number_type::template apply<Context>::type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;

    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    SVGPP_STATIC_IF_SAFE const qi::real_parser<coordinate_t, detail::number_policies<coordinate_t, tag::source::attribute> > number;
    SVGPP_STATIC_IF_SAFE const detail::comma_wsp_rule<iterator_t> comma_wsp;
    coordinate_t value1, value2;
    bool two_values = false;
    if (qi::parse(it, end, 
          number [ref(value1) = _1] 
          >> -(
              comma_wsp 
              >> number [ref(value2) = _1, ref(two_values) = true]
              )
      ) && it == end)
    {
      if (two_values)
        context_policy<tag::load_value_policy, Context>::set(context, tag, value1, value2);
      else
        context_policy<tag::load_value_policy, Context>::set(context, tag, value1);
      return true;
    }
    else
    {
      return args_t::get_error_policy::template apply<Context>::type::parse_failed(context, tag, attribute_value);
    }
  }
};


}
