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
  static BOOST_CONSTEXPR bool parse_inf(Iterator&, Iterator const&, Attribute&) { return false; }
  template <typename Iterator, typename Attribute>
  static BOOST_CONSTEXPR bool parse_nan(Iterator&, Iterator const&, Attribute&) { return false; }
};

template<class Coordinate, class PropertySource>
struct number_policies;

template<class Coordinate>
struct number_policies<Coordinate, tag::source::attribute>: svg_real_policies< Coordinate >
{
};

template<class Coordinate>
struct number_policies<Coordinate, tag::source::css>: svg_real_policies< Coordinate >
{
  template <typename Iterator>
  static BOOST_CONSTEXPR bool parse_exp(Iterator& first, Iterator const&)
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
