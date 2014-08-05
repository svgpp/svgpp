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
  class AngleFactory, 
  class Number = double
>
class angle_grammar:
  public qi::grammar<Iterator, typename AngleFactory::angle_type(), qi::locals<Number> >
{
  typedef angle_grammar<PropertySource, Iterator, AngleFactory, Number> this_type;
public:
  typedef typename AngleFactory::angle_type angle_type; 

  angle_grammar()
    : this_type::grammar(rule_)
  {
    namespace phx = boost::phoenix;
    using qi::_1;
    using qi::_a;
    using qi::_val;
    using qi::lit;

    rule_ 
        =   number_ [_a = _1] 
            >>  detail::no_case_if_css(PropertySource())
                [
                    ( lit("grad")
                          [_val = phx::bind(&angle_grammar::call_make_angle<tag::angle_units::grad>, _a)]
                    | lit("rad")
                          [_val = phx::bind(&angle_grammar::call_make_angle<tag::angle_units::rad>, _a)]
                    | ( -lit("deg") )
                          [_val = phx::bind(&angle_grammar::call_make_angle<tag::angle_units::deg>, _a)]
                    )
                ];
  }

private:
  typename this_type::start_type rule_; 
  qi::real_parser<Number, detail::number_policies<Number, PropertySource> > number_;

  template<class UnitsTag>
  static angle_type call_make_angle(Number value) 
  {
    return AngleFactory::create(value, UnitsTag());
  }
};

}