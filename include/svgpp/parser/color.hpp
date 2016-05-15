// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/external_function/parse_color.hpp>
#if !defined(SVGPP_USE_EXTERNAL_COLOR_PARSER)
# include <svgpp/parser/external_function/parse_color_impl.hpp>
#endif

namespace svgpp
{

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::color, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source)
  {
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::color_factory>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename detail::unwrap_context<Context, tag::color_factory>::template bind<args2_t>::type color_factory_t;

    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    typename color_factory_t::color_type color;
    if (detail::parse_color<color_factory_t>(it, end, property_source, color) && it == end)
    {
      args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source, color);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::color_optional_icc_color, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source)
  {
    namespace qi = boost::spirit::qi;

    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::color_factory>,
      boost::parameter::optional<tag::icc_color_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename detail::unwrap_context<Context, tag::color_factory>::template bind<args2_t>::type color_factory_t;
    typedef detail::unwrap_context<Context, tag::icc_color_policy> icc_color_context_t;
    typedef typename icc_color_context_t::template bind<args2_t>::type icc_color_policy_t;
    typedef typename icc_color_policy_t::icc_color_factory_type icc_color_factory_t;

    icc_color_factory_t & icc_color_factory = icc_color_policy_t::icc_color_factory(icc_color_context_t::get(context));
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    boost::tuple<typename color_factory_t::color_type, boost::optional<typename icc_color_factory_t::icc_color_type> > color;
    if (detail::parse_color_optional_icc_color<color_factory_t>(icc_color_factory, it, end, property_source, color)
      && it == end)
    {
      if (color.template get<1>())
        args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source, color.template get<0>(), *color.template get<1>());
      else
        args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source, color.template get<0>());
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

}