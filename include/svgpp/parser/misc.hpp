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
#include <svgpp/parser/grammar/length.hpp>
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
    namespace qi = boost::spirit::qi;
    using namespace boost::phoenix;
    using qi::_1;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::number_type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);

    SVGPP_STATIC_IF_SAFE const qi::real_parser<coordinate_t, detail::svg_real_policies<coordinate_t> > number;
    SVGPP_STATIC_IF_SAFE const detail::comma_wsp_rule_no_skip<iterator_t> comma_wsp;
    coordinate_t x, y, w, h;
    if (qi::parse(it, end, 
      number[ref(x) = _1] >> comma_wsp >>
      number[ref(y) = _1] >> comma_wsp >>
      number[ref(w) = _1] >> comma_wsp >>
      number[ref(h) = _1]) && it == end)
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
    namespace qi = boost::spirit::qi;
    using namespace boost::phoenix;
    using qi::_1;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::number_type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);

    SVGPP_STATIC_IF_SAFE const qi::real_parser<coordinate_t, detail::svg_real_policies<coordinate_t> > number;
    coordinate_t lo_x, lo_y, hi_x, hi_y;
    if (qi::parse(it, end, 
      number[ref(lo_x) = _1] >> qi::lit(',') >>
      number[ref(lo_y) = _1] >> qi::lit(',') >>
      number[ref(hi_x) = _1] >> qi::lit(',') >>
      number[ref(hi_y) = _1]) && it == end)
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
    namespace qi = boost::spirit::qi;
    namespace phx = boost::phoenix;
    using qi::_1;
    using qi::_a;
    using qi::_b;
    using qi::_c;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    qi::rule<iterator_t, qi::locals<bool, int, bool> > rule 
        =   (  -( qi::lit("defer") [_a = true]
                  >> +qi::ascii::space
                )
            >>  ( qi::lit("none") [_b = 1]
                | qi::lit("xMinYMin") [_b = 2]
                | qi::lit("xMidYMin") [_b = 3]
                | qi::lit("xMaxYMin") [_b = 4]
                | qi::lit("xMinYMid") [_b = 5]
                | qi::lit("xMidYMid") [_b = 6]
                | qi::lit("xMaxYMid") [_b = 7]
                | qi::lit("xMinYMax") [_b = 8]
                | qi::lit("xMidYMax") [_b = 9]
                | qi::lit("xMaxYMax") [_b = 10]
                )
            >> -( +qi::ascii::space
                >> ( qi::lit("meet") [_c = false]
                   | qi::lit("slice") [_c = true] 
                   ) 
                )
            ) [phx::bind(&value_parser::call_set_value<Context>, phx::ref(context), _a, _b, _c)];
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    if (qi::parse(it, end, rule) && it == end)
      return true;
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }

  template<class Context>
  static void call_set_value(Context & context, bool defer, int align, bool slice)
  {
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::value_events_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename detail::unwrap_context<Context, tag::value_events_policy> unwrap_value_events_policy_t;
    typedef typename unwrap_value_events_policy_t::template bind<args2_t>::type value_events_policy_t;
    switch (align)
    {
    case 1:
      value_events_policy_t::set(unwrap_value_events_policy_t::get(context), 
        tag::attribute::preserveAspectRatio(), 
        tag::source::attribute(),
        defer, tag::value::none());
      break;
#define SVGPP_CASE(value_tag) \
      if (slice) \
        value_events_policy_t::set(unwrap_value_events_policy_t::get(context), \
          tag::attribute::preserveAspectRatio(), tag::source::attribute(), \
          defer, tag::value::value_tag(), tag::value::slice()); \
      else \
        value_events_policy_t::set(unwrap_value_events_policy_t::get(context), \
          tag::attribute::preserveAspectRatio(), tag::source::attribute(), \
          defer, tag::value::value_tag(), tag::value::meet()); \
      break;

      case 2: SVGPP_CASE(xMinYMin);
      case 3: SVGPP_CASE(xMidYMin);
      case 4: SVGPP_CASE(xMaxYMin);
      case 5: SVGPP_CASE(xMinYMid);
      case 6: SVGPP_CASE(xMidYMid);
      case 7: SVGPP_CASE(xMaxYMid);
      case 8: SVGPP_CASE(xMinYMax);
      case 9: SVGPP_CASE(xMidYMax);
      case 10: SVGPP_CASE(xMaxYMax);
#undef SVGPP_CASE
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
    using namespace boost::phoenix;
    using qi::_1;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::number_type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);

    SVGPP_STATIC_IF_SAFE const qi::real_parser<coordinate_t, detail::number_policies<coordinate_t, tag::source::attribute> > number;
    coordinate_t value;
    bool percentage = false;
    if (qi::parse(it, end, number[ref(value) = _1] >> -(qi::lit('%')[ref(percentage) = true])) && it == end)
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

    namespace qi = boost::spirit::qi;
    namespace phx = boost::phoenix;
    using qi::_1;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::length_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename detail::unwrap_context<Context, tag::length_policy> length_policy_context;
    typedef typename length_policy_context::template bind<args2_t>::type length_policy_t;
    typename length_policy_t::length_factory_type & length_factory 
      = length_policy_t::length_factory(length_policy_context::get(context));
    SVGPP_STATIC_IF_SAFE const length_grammar<
      PropertySource, 
      iterator_t, 
      typename length_policy_t::length_factory_type, 
      tag::length_dimension::width
    > length_grammar_x;
    SVGPP_STATIC_IF_SAFE const length_grammar<
      PropertySource, 
      iterator_t, 
      typename length_policy_t::length_factory_type, 
      tag::length_dimension::height
    > length_grammar_y;

    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    typename length_policy_t::length_factory_type::length_type rect[4];
    // Initializing with values for 'auto'
    for(int i=0; i<4; ++i)
      rect[i] = length_factory.create_length(0, tag::length_units::none()); 
    const qi::rule<iterator_t> rule = 
      detail::no_case_if_css(property_source)
      [
        qi::lit("rect") >> *detail::character_encoding_namespace::space >> qi::lit('(')
        >> *detail::character_encoding_namespace::space
        >>  ( qi::lit("auto") 
            | length_grammar_y(phx::cref(length_factory)) [phx::ref(rect[0]) = qi::_1]
            )
        >> +detail::character_encoding_namespace::space
        >>  ( qi::lit("auto")
            | length_grammar_x(phx::cref(length_factory)) [phx::ref(rect[1]) = qi::_1]
            )
        >> +detail::character_encoding_namespace::space
        >>  ( qi::lit("auto")
            | length_grammar_y(phx::cref(length_factory)) [phx::ref(rect[2]) = qi::_1]
            )
        >> +detail::character_encoding_namespace::space
        >>  ( qi::lit("auto")
            | length_grammar_x(phx::cref(length_factory)) [phx::ref(rect[3]) = qi::_1]
            )
        >> *detail::character_encoding_namespace::space >> qi::lit(')')
      ];
    if (qi::parse(it, end, rule) && it == end)
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

    namespace qi = boost::spirit::qi;
    namespace phx = boost::phoenix;
    using qi::_1;

    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef typename args_t::number_type coordinate_t;

    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    SVGPP_STATIC_IF_SAFE const qi::real_parser<coordinate_t, detail::number_policies<coordinate_t, PropertySource> > number;
    coordinate_t x, y, width, height;
    const qi::rule<iterator_t> rule = 
      detail::no_case_if_css(property_source)[ qi::lit("new") ]
      >> +detail::character_encoding_namespace::space 
      >> number [phx::ref(x) = qi::_1]
      >> +detail::character_encoding_namespace::space 
      >> number [phx::ref(y) = qi::_1]
      >> +detail::character_encoding_namespace::space 
      >> number [phx::ref(width) = qi::_1]
      >> +detail::character_encoding_namespace::space 
      >> number [phx::ref(height) = qi::_1];
    // TODO: check for negative values
    if (qi::parse(it, end, rule) && it == end)
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