// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/config.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/parser/detail/common.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/external_function/parse_preserveAspectRatio.hpp>
#if !defined(SVGPP_USE_EXTERNAL_PRESERVE_ASPECT_RATIO_PARSER)
# include <svgpp/parser/external_function/parse_preserveAspectRatio_impl.hpp>
#endif
#include <svgpp/parser/external_function/parse_misc.hpp>
#if !defined(SVGPP_USE_EXTERNAL_MISC_PARSER)
# include <svgpp/parser/external_function/parse_misc_impl.hpp>
#endif
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/policy/value_events.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace svgpp
{

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::integer, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source)
  {
    namespace qi = boost::spirit::qi;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);

    int value;
    if (qi::parse(it, end, qi::int_, value) && it == end)
    {
      args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source, value);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::attribute::viewBox, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class Context, class AttributeValue>
  static bool parse(tag::attribute::viewBox tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::attribute property_source)
  {
    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::number_type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);

    coordinate_t x, y, w, h;
    if (detail::parse_viewBox(it, end, x, y, w, h) && it == end)
    {
      args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source, x, y, w, h);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::attribute::bbox, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class Context, class AttributeValue>
  static bool parse(tag::attribute::bbox tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::attribute property_source)
  {
    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::number_type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);

    SVGPP_STATIC_IF_SAFE const qi::real_parser<coordinate_t, detail::svg_real_policies<coordinate_t> > number;
    coordinate_t lo_x, lo_y, hi_x, hi_y;
    if (detail::parse_bbox(it, end, lo_x, lo_y, hi_x, hi_y) && it == end)
    {
      args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source, lo_x, lo_y, hi_x, hi_y);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::attribute::preserveAspectRatio, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class Context, class AttributeValue>
  static bool parse(tag::attribute::preserveAspectRatio tag, Context & context, 
    AttributeValue const & attribute_value, tag::source::attribute)
  {
    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;

    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    preserveAspectRatio_value value;
    if (detail::parse_preserveAspectRatio(it, end, value) && it == end)
    {
      typedef typename boost::parameter::parameters<
        boost::parameter::optional<tag::value_events_policy>
      >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
      typedef typename detail::unwrap_context<Context, tag::value_events_policy> unwrap_value_events_policy_t;
      typedef typename unwrap_value_events_policy_t::template bind<args2_t>::type value_events_policy_t;

      switch (value.align)
      {
        case preserveAspectRatio_value::none:
        value_events_policy_t::set(unwrap_value_events_policy_t::get(context), 
          tag::attribute::preserveAspectRatio(), 
          tag::source::attribute(),
          value.defer, tag::value::none());
        break;

#define SVGPP_CASE(value_tag) \
        case preserveAspectRatio_value::value_tag: \
        if (value.slice) \
          value_events_policy_t::set(unwrap_value_events_policy_t::get(context), \
            tag::attribute::preserveAspectRatio(), tag::source::attribute(), \
            value.defer, tag::value::value_tag(), tag::value::slice()); \
        else \
          value_events_policy_t::set(unwrap_value_events_policy_t::get(context), \
            tag::attribute::preserveAspectRatio(), tag::source::attribute(), \
            value.defer, tag::value::value_tag(), tag::value::meet()); \
        break;

        SVGPP_CASE(xMinYMin);
        SVGPP_CASE(xMidYMin);
        SVGPP_CASE(xMaxYMin);
        SVGPP_CASE(xMinYMid);
        SVGPP_CASE(xMidYMid);
        SVGPP_CASE(xMaxYMid);
        SVGPP_CASE(xMinYMax);
        SVGPP_CASE(xMidYMax);
        SVGPP_CASE(xMaxYMax);
#undef SVGPP_CASE
      }
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<boost::mpl::pair<tag::element::stop, tag::attribute::offset>, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class Context, class AttributeValue>
  static bool parse(tag::attribute::offset tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::attribute property_source)
  {
    namespace qi = boost::spirit::qi;
    namespace phx = boost::phoenix;
    using qi::_1;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::number_type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);

    SVGPP_STATIC_IF_SAFE const qi::real_parser<coordinate_t, detail::number_policies<coordinate_t, tag::source::attribute> > number;
    coordinate_t value;
    bool percentage = false;
    if (qi::parse(it, end, number[phx::ref(value) = _1] >> -(qi::lit('%')[phx::ref(percentage) = true])) && it == end)
    {
      args_t::value_events_policy::set(args_t::value_events_context::get(context), 
        tag, property_source, percentage ? value / 100 : value);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::attribute::clip, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class Context, class AttributeValue, class PropertySource>
  static bool parse(tag::attribute::clip tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source)
  {
    // 'auto' and 'inherit' values must be checked outside

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::length_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename detail::unwrap_context<Context, tag::length_policy> length_policy_context;
    typedef typename length_policy_context::template bind<args2_t>::type length_policy_t;
    typename length_policy_t::length_factory_type & length_factory 
      = length_policy_t::length_factory(length_policy_context::get(context));

    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    typename length_policy_t::length_factory_type::length_type rect[4];
    if (detail::parse_clip(length_factory, it, end, property_source, rect) && it == end)
    {
      args_t::value_events_policy::set(args_t::value_events_context::get(context), 
        tag, property_source, tag::value::rect(), rect[0], rect[1], rect[2], rect[3]);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::attribute::enable_background, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class Context, class AttributeValue, class PropertySource>
  static bool parse(tag::attribute::enable_background tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source)
  {
    // 'accumulate', 'new' and 'inherit' values must be checked outside

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef typename args_t::number_type coordinate_t;

    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    coordinate_t x, y, width, height;
    if (detail::parse_enable_background(it, end, property_source, x, y, width, height) && it == end)
    {
      args_t::value_events_policy::set(args_t::value_events_context::get(context), 
        tag, property_source, tag::value::new_(), x, y, width, height);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::attribute::text_decoration, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class Context, class AttributeValue, class PropertySource>
  static bool parse(tag::attribute::text_decoration tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource property_source)
  {
    // 'none' and 'inherit' values must be checked outside

    namespace qi = boost::spirit::qi;
    namespace phx = boost::phoenix;
    using qi::_1;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;

    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    bool underline = false, overline = false, line_through = false, blink = false;
    const qi::rule<iterator_t> value = 
      detail::no_case_if_css(property_source)
      [
        qi::lit("underline")      [phx::ref(underline) = true]
        | qi::lit("overline")     [phx::ref(overline) = true]
        | qi::lit("line-through") [phx::ref(line_through) = true]
        | qi::lit("blink")        [phx::ref(blink) = true]
      ];

    const qi::rule<iterator_t> rule = value % +detail::character_encoding_namespace::space;
    if (qi::parse(it, end, rule) && it == end)
    {
      args_t::value_events_policy::set(args_t::value_events_context::get(context), tag, property_source,
        tag::value::underline(), underline,
        tag::value::overline(), overline,
        tag::value::line_through(), line_through,
        tag::value::blink(), blink);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

}