// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/factory/color.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/detail/pass_iri_value.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/external_function/parse_paint.hpp>
#if !defined(SVGPP_USE_EXTERNAL_PAINT_PARSER)
# include <svgpp/parser/external_function/parse_paint_impl.hpp>
#endif
#include <svgpp/policy/iri.hpp>
#include <svgpp/policy/value_events.hpp>

namespace svgpp
{

  template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::paint, SVGPP_TEMPLATE_ARGS_PASS>
  {
    template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
    static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                      PropertySource property_source)
    {
      typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
      typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
      typedef typename boost::parameter::parameters<
        boost::parameter::optional<tag::color_factory>,
        boost::parameter::optional<tag::icc_color_policy>,
        boost::parameter::optional<tag::iri_policy>
      >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
      typedef typename detail::unwrap_context<Context, tag::color_factory>::template bind<args2_t>::type color_factory_t;
      typedef detail::unwrap_context<Context, tag::icc_color_policy> icc_color_context_t;
      typedef typename icc_color_context_t::template bind<args2_t>::type icc_color_policy_t;
      typedef typename icc_color_policy_t::icc_color_factory_type icc_color_factory_t;
      typedef typename detail::unwrap_context<Context, tag::iri_policy>::template bind<args2_t>::type iri_policy_t;
      typedef typename args_t::value_events_policy value_events_policy_t;
      typename args_t::value_events_context::type & value_events_context = args_t::value_events_context::get(context);
      typedef typename detail::value_events_with_iri_policy<value_events_policy_t, iri_policy_t>::type
        value_events_with_iri_policy_t;

      iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
      detail::paint_option main_option, funciri_suboption = detail::paint_option_not_set;
      boost::tuple<
        typename color_factory_t::color_type, boost::optional<typename icc_color_factory_t::icc_color_type>
      > color;
      typename boost::iterator_range<iterator_t> iri;
      if (detail::parse_paint<color_factory_t>(
          icc_color_policy_t::icc_color_factory(icc_color_context_t::get(context)),
          it, end, property_source,
          main_option, funciri_suboption,
          color, iri)
        && it == end)
      {
        switch (main_option)
        {
        default:
          // Just suppressing warning
          break;
        case detail::paint_option_none:
          value_events_policy_t::set(value_events_context, tag, property_source, tag::value::none());
          break;
        case detail::paint_option_currentColor:
          value_events_policy_t::set(value_events_context, tag, property_source, tag::value::currentColor());
          break;
        case detail::paint_option_inherit:
          value_events_policy_t::set(value_events_context, tag, property_source, tag::value::inherit());
          break;
        case detail::paint_option_color:
          if (color.template get<1>())
            value_events_policy_t::set(value_events_context, tag, property_source, color.template get<0>(), *color.template get<1>());
          else
            value_events_policy_t::set(value_events_context, tag, property_source, color.template get<0>());
          break;
        case detail::paint_option_funciri:
          switch (funciri_suboption)
          {
          default:
            // Just suppressing warning
            break;
          case detail::paint_option_not_set:
            value_events_with_iri_policy_t::set(value_events_context, tag, property_source, iri);
            break;
          case detail::paint_option_none:
            value_events_with_iri_policy_t::set(value_events_context, tag, property_source, iri, tag::value::none());
            break;
          case detail::paint_option_currentColor:
            value_events_with_iri_policy_t::set(value_events_context, tag, property_source, iri, tag::value::currentColor());
            break;
          case detail::paint_option_color:
            if (color.template get<1>())
              value_events_with_iri_policy_t::set(value_events_context, tag, property_source, iri, color.template get<0>(), *color.template get<1>());
            else
              value_events_with_iri_policy_t::set(value_events_context, tag, property_source, iri, color.template get<0>());
            break;
          }
          break;
        }
        return true;
      }
      else
      {
        return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
      }
    }
  };

}