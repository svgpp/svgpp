// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <svgpp/parser/detail/common.hpp>

namespace svgpp
{ 
  
namespace qi = boost::spirit::qi;

template <class Iterator, class Coordinate = double>
class coordinate_pair_grammar: 
  public qi::grammar<Iterator, std::pair<Coordinate, Coordinate>(), qi::locals<Coordinate> >
{
public:
  coordinate_pair_grammar()
    : coordinate_pair_grammar::base_type(rule_)
  {
    rule_ = 
         number [qi::_a = qi::_1]
      >> (   comma_wsp
           | &qi::lit('-')
         )
      >> number
        [qi::_val = boost::phoenix::bind(&coordinate_pair_grammar::make_pair, qi::_a, qi::_1)];
  }

private:
  typename coordinate_pair_grammar::start_type rule_;
  detail::comma_wsp_rule_no_skip<Iterator> comma_wsp;
  qi::real_parser<Coordinate, detail::real_policies_without_inf_nan<Coordinate> > number; // trailing dot is allowed, 'inf' and 'nan' - no

  static std::pair<Coordinate, Coordinate> make_pair(Coordinate val1, Coordinate val2)
  {
    return std::pair<Coordinate, Coordinate>(val1, val2);
  }
};

}