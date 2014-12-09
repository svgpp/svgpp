// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace svgpp
{

namespace qi = boost::spirit::qi;

namespace detail
{

template<class Number>
struct real_policies_seconds_with_fraction: qi::ureal_policies<Number>
{
  static bool const allow_trailing_dot = false;
  static bool const allow_leading_dot = false;

  template <typename Iterator, typename Attribute>
  static BOOST_CONSTEXPR bool parse_inf(Iterator &, Iterator const &, Attribute &) { return false; }
  template <typename Iterator, typename Attribute>
  static BOOST_CONSTEXPR bool parse_nan(Iterator &, Iterator const &, Attribute &) { return false; }
  template <typename Iterator>
  static BOOST_CONSTEXPR bool parse_exp(Iterator &, Iterator const &) { return false; }
  template <typename Iterator, typename Attribute>
  static bool parse_n(Iterator & first, Iterator const & last, Attribute & attr) 
  { 
    Iterator f = first;
    if (!qi::ureal_policies<Number>::parse_n(f, last, attr))
      return false;
    if (std::distance(first, f) == 2 && attr <= 59)
    {
      first = f;
      return true;
    }
    return false; 
  }
};

template<class Number>
struct real_policies_timecount: qi::ureal_policies<Number>
{
  static bool const allow_trailing_dot = false;
  static bool const allow_leading_dot = false;

  template <typename Iterator, typename Attribute>
  static BOOST_CONSTEXPR bool parse_inf(Iterator &, Iterator const &, Attribute &) { return false; }
  template <typename Iterator, typename Attribute>
  static BOOST_CONSTEXPR bool parse_nan(Iterator &, Iterator const &, Attribute &) { return false; }
  template <typename Iterator>
  static BOOST_CONSTEXPR bool parse_exp(Iterator &, Iterator const &) { return false; }
};

}

template <class Iterator, class TimeT = double>
class clock_value_grammar: public qi::grammar<Iterator, TimeT ()>
{
  typedef clock_value_grammar<Iterator, TimeT> this_type;

  enum metric {em, ex, px, in, cm, mm, pt, pc};
public:
  clock_value_grammar()
    : this_type::grammar(rule_)
  {
    namespace phx = boost::phoenix;
    using qi::lit;
    using qi::_1;
    using qi::_2;
    using qi::_3;
    using qi::_a;
    using qi::_val;

    rule_ 
        =   full_clock_rule_
        |   partial_clock_rule_
        |   timecount_rule_;

    full_clock_rule_ 
        =   (  qi::uint_ 
            >> lit(':') 
            >> minutes_rule_
            >> lit(':') 
            >> seconds_rule_
            ) [_val = phx::bind(&clock_value_grammar::create_clock, _1, _2, _3)];

    partial_clock_rule_ 
        =   (  minutes_rule_
            >> lit(':') 
            >> seconds_rule_
            ) [_val = phx::bind(&clock_value_grammar::create_clock, 0, _1, _2)];

    timecount_rule_
        =   timecount_number_rule_ [_a = _1]
            >>  (   lit('h')    [_val = phx::bind(&clock_value_grammar::create_hours, _a)]
                |   lit("min")  [_val = phx::bind(&clock_value_grammar::create_minutes, _a)]
                |   lit('s')    [_val = _a]
                |   lit("ms")   [_val = phx::bind(&clock_value_grammar::create_ms, _a)]
                |   qi::eps     [_val = _a] 
                );
  }

private:
  typename this_type::start_type 
    rule_, 
    full_clock_rule_, 
    partial_clock_rule_;
  qi::rule<Iterator, TimeT (), qi::locals<TimeT> > timecount_rule_;
  qi::uint_parser<unsigned, 10, 2, 2> minutes_rule_;
  qi::real_parser<TimeT, detail::real_policies_seconds_with_fraction<TimeT> > seconds_rule_;
  qi::real_parser<TimeT, detail::real_policies_timecount<TimeT> > timecount_number_rule_;

  static TimeT create_clock(unsigned hours, unsigned minutes, TimeT seconds)
  {
    return (hours * 60 + minutes) * 60 + seconds;
  }

  static TimeT create_hours(TimeT value)
  {
    return value * 3600;
  }

  static TimeT create_minutes(TimeT value)
  {
    return value * 60;
  }

  static TimeT create_ms(TimeT value)
  {
    return value / 1000;
  }
};

}