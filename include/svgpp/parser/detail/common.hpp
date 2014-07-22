// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/spirit/include/qi.hpp>
#include <svgpp/definitions.hpp>

namespace svgpp { namespace detail 
{ 

namespace character_encoding_namespace = boost::spirit::ascii;

template<class Coordinate>
struct svg_real_policies: boost::spirit::qi::real_policies<Coordinate>
{
  static bool const allow_trailing_dot = false;

  template <typename Iterator, typename Attribute>
  static BOOST_CONSTEXPR bool parse_inf(Iterator &, Iterator const &, Attribute &) { return false; }
  template <typename Iterator, typename Attribute>
  static BOOST_CONSTEXPR bool parse_nan(Iterator &, Iterator const &, Attribute &) { return false; }
};

template<class Coordinate, class PropertySource>
struct number_policies;

template<class Coordinate>
struct number_policies<Coordinate, tag::source::attribute>: svg_real_policies<Coordinate>
{
  template <typename Iterator>
  static bool parse_exp(Iterator & first, Iterator const & last)
  {
    // Check that "e" is followed by integer to be able to parse something like "4em" correctly
    Iterator it = first;
    if (svg_real_policies<Coordinate>::parse_exp(it, last))
    {
      // Do some prefetch before accepting "e" as start of the exponent part
      Iterator it2 = it;
      int exp_val;
      if (svg_real_policies<Coordinate>::parse_exp_n(it2, last, exp_val))
      {
        first = it;
        return true;
      }
    }
    return false;
  }
};

template<class Coordinate>
struct number_policies<Coordinate, tag::source::css>: svg_real_policies<Coordinate>
{
  template <typename Iterator>
  static BOOST_CONSTEXPR bool parse_exp(Iterator & first, Iterator const &)
  {
    return false;
  }
};

template<class Iterator>
struct comma_wsp_rule: boost::spirit::qi::rule<Iterator>
{
  comma_wsp_rule()
    : boost::spirit::qi::rule<Iterator>(
        boost::spirit::qi::lit(',') 
      | (  +character_encoding_namespace::space 
        >> -boost::spirit::qi::lit(',')))
  {
  }
};

template<class Iterator>
struct comma_wsp_rule_no_skip: boost::spirit::qi::rule<Iterator>
{
  comma_wsp_rule_no_skip()
    : boost::spirit::qi::rule<Iterator>(
        (   boost::spirit::qi::lit(',') 
        >> *character_encoding_namespace::space
        )
      | (  +character_encoding_namespace::space
        >> -(   boost::spirit::qi::lit(',') 
            >> *character_encoding_namespace::space
            )
        )
      )
  {
  }
};

}}
