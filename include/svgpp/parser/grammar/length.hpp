// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/parser/detail/common.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_grammar.hpp>

namespace svgpp 
{

namespace qi = boost::spirit::qi;

template <
  class PropertySource, 
  class Iterator, 
  class LengthFactory, 
  class PercentageDirectionTag = tag::not_width_nor_height_length,
  class Number = typename LengthFactory::number_type
>
class length_grammar;

template <typename Iterator, class LengthFactory, class PercentageDirectionTag, class Number>
class length_grammar<tag::source::attribute, Iterator, LengthFactory, PercentageDirectionTag, Number>:
  public qi::grammar<Iterator, typename LengthFactory::length_type(LengthFactory const &), qi::locals<Number> >
{
  typedef length_grammar<tag::source::attribute, Iterator, LengthFactory, PercentageDirectionTag, Number> this_type;
public:
  typedef typename LengthFactory::length_type length_type; 

  length_grammar()
    : this_type::grammar(rule_)
  {
    namespace phx = boost::phoenix;
    using qi::_1;
    using qi::_a;
    using qi::_val;
    using qi::_r1;
    rule_ 
        =   number_ [_a = _1] 
            >>  ( qi::lit("em")
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::em>, _r1, _a)]
                | qi::lit("ex")
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::ex>, _r1, _a)]
                | qi::lit("px")
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::px>, _r1, _a)]
                | qi::lit("in")
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::in>, _r1, _a)]
                | qi::lit("cm")
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::cm>, _r1, _a)]
                | qi::lit("mm")
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::mm>, _r1, _a)]
                | qi::lit("pt")
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::pt>, _r1, _a)]
                | qi::lit("pc")
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::pc>, _r1, _a)]
                | qi::lit("%")
                      [_val = phx::bind(&length_grammar::call_make_length_percent, _r1, _a)]
                | qi::eps
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::none>, _r1, _a)]
                );
  }

private:
  typename this_type::start_type rule_; 
  qi::real_parser<Number, detail::number_policies<Number, tag::source::attribute> > number_;

  template<class UnitsTag>
  static length_type call_make_length(LengthFactory const & length_factory, Number value)
  {
    return length_factory.create_length(value, UnitsTag());
  }

  static length_type call_make_length_percent(LengthFactory const & length_factory, Number value)
  {
    return length_factory.create_length(value, tag::length_units::percent(), PercentageDirectionTag());
  }
};

template<
  class Iterator, 
  class LengthFactory, 
  class PercentageDirectionTag, 
  class Number
>
class length_grammar<tag::source::css, Iterator, LengthFactory, PercentageDirectionTag, Number>:
  public qi::grammar<Iterator, typename LengthFactory::length_type(LengthFactory const &), qi::locals<Number> >
{
  typedef length_grammar<tag::source::css, Iterator, LengthFactory, PercentageDirectionTag, Number> this_type;
public:
  typedef typename LengthFactory::length_type length_type; 

  length_grammar()
    : this_type::grammar(rule_)
  {
    namespace phx = boost::phoenix;
    using qi::_1;
    using qi::_a;
    using qi::_val;
    using qi::_r1;
    rule_ 
        =   number_ [_a = _1] 
            >>  ( ( qi::lit("em") | qi::lit("EM") )
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::em>, _r1, _a)]
                | ( qi::lit("ex") | qi::lit("EX") )
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::ex>, _r1, _a)]
                | ( qi::lit("px") | qi::lit("PX") )
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::px>, _r1, _a)]
                | ( qi::lit("in") | qi::lit("IN") )
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::in>, _r1, _a)]
                | ( qi::lit("cm") | qi::lit("CM") )
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::cm>, _r1, _a)]
                | ( qi::lit("mm") | qi::lit("MM") )
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::mm>, _r1, _a)]
                | ( qi::lit("pt") | qi::lit("PT") )
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::pt>, _r1, _a)]
                | ( qi::lit("pc") | qi::lit("PC") )
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::pc>, _r1, _a)]
                | qi::eps
                      [_val = phx::bind(&length_grammar::call_make_length<tag::length_units::none>, _r1, _a)]
                );
  }

private:
  typename this_type::start_type rule_; 
  qi::real_parser<Number, detail::number_policies<Number, tag::source::css> > number_;

  template<class UnitsTag>
  static length_type call_make_length(LengthFactory const & length_factory, Number value)
  {
    return length_factory.create_length(value, UnitsTag());
  }
};

template <
  class Iterator, 
  class LengthFactory, 
  class PercentageDirectionTag, 
  class Number = typename LengthFactory::number_type
>
class percentage_or_length_css_grammar:
  public qi::grammar<Iterator, typename LengthFactory::length_type(LengthFactory const &), qi::locals<Number> >
{
  typedef percentage_or_length_css_grammar<Iterator, LengthFactory, PercentageDirectionTag, Number> this_type;
public:
  typedef typename LengthFactory::length_type length_type; 

  percentage_or_length_css_grammar()
    : this_type::grammar(rule_)
  {
    namespace phx = boost::phoenix;
    using qi::_1;
    using qi::_a;
    using qi::_val;
    using qi::_r1;
    rule_ 
        =   number_ [_a = _1] 
            >>  ( ( qi::lit("em") | qi::lit("EM") )
                      [_val = phx::bind(&this_type::call_make_length<tag::length_units::em>, _r1, _a)]
                | ( qi::lit("ex") | qi::lit("EX") )
                      [_val = phx::bind(&this_type::call_make_length<tag::length_units::ex>, _r1, _a)]
                | ( qi::lit("px") | qi::lit("PX") )
                      [_val = phx::bind(&this_type::call_make_length<tag::length_units::px>, _r1, _a)]
                | ( qi::lit("in") | qi::lit("IN") )
                      [_val = phx::bind(&this_type::call_make_length<tag::length_units::in>, _r1, _a)]
                | ( qi::lit("cm") | qi::lit("CM") )
                      [_val = phx::bind(&this_type::call_make_length<tag::length_units::cm>, _r1, _a)]
                | ( qi::lit("mm") | qi::lit("MM") )
                      [_val = phx::bind(&this_type::call_make_length<tag::length_units::mm>, _r1, _a)]
                | ( qi::lit("pt") | qi::lit("PT") )
                      [_val = phx::bind(&this_type::call_make_length<tag::length_units::pt>, _r1, _a)]
                | ( qi::lit("pc") | qi::lit("PC") )
                      [_val = phx::bind(&this_type::call_make_length<tag::length_units::pc>, _r1, _a)]
                | qi::lit("%")
                      [_val = phx::bind(&this_type::call_make_length_percent, _r1, _a)]
                | qi::eps
                      [_val = phx::bind(&this_type::call_make_length<tag::length_units::none>, _r1, _a)]
                );
  }

private:
  typename this_type::start_type rule_; 
  qi::real_parser<Number, detail::number_policies<Number, tag::source::css> > number_;

  template<class UnitsTag>
  static length_type call_make_length(LengthFactory const & length_factory, Number value)
  {
    return length_factory.create_length(value, UnitsTag());
  }

  static length_type call_make_length_percent(LengthFactory const & length_factory, Number value)
  {
    return length_factory.create_length(value, tag::length_units::percent(), PercentageDirectionTag());
  }
};

}