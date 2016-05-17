// Copyright Oleg Maximenko 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_grammar.hpp>

namespace svgpp 
{

namespace qi = boost::spirit::qi;

struct preserveAspectRatio_value
{
  enum align_type
  {
    none,
    xMinYMin,
    xMidYMin,
    xMaxYMin,
    xMinYMid,
    xMidYMid,
    xMaxYMid,
    xMinYMax,
    xMidYMax,
    xMaxYMax
  };

  bool  defer;
  align_type align; 
  bool  slice;
};

template <class Iterator>
class preserveAspectRatio_grammar:
  public qi::grammar<Iterator, preserveAspectRatio_value(), qi::locals<bool, preserveAspectRatio_value::align_type, bool> >
{
public:
  preserveAspectRatio_grammar()
    : preserveAspectRatio_grammar::grammar(rule_)
  {
    using qi::_a;
    using qi::_b;
    using qi::_c;
    using qi::_val;
    using qi::lit;
    namespace phx = boost::phoenix;

    rule_
      = (-(qi::lit("defer")[_a = true]
        >> +qi::ascii::space
        )
        >> (qi::lit("none")    [_b = preserveAspectRatio_value::none]
          | qi::lit("xMinYMin")[_b = preserveAspectRatio_value::xMinYMin]
          | qi::lit("xMidYMin")[_b = preserveAspectRatio_value::xMidYMin]
          | qi::lit("xMaxYMin")[_b = preserveAspectRatio_value::xMaxYMin]
          | qi::lit("xMinYMid")[_b = preserveAspectRatio_value::xMinYMid]
          | qi::lit("xMidYMid")[_b = preserveAspectRatio_value::xMidYMid]
          | qi::lit("xMaxYMid")[_b = preserveAspectRatio_value::xMaxYMid]
          | qi::lit("xMinYMax")[_b = preserveAspectRatio_value::xMinYMax]
          | qi::lit("xMidYMax")[_b = preserveAspectRatio_value::xMidYMax]
          | qi::lit("xMaxYMax")[_b = preserveAspectRatio_value::xMaxYMax]
          )
        >> -(+qi::ascii::space
          >> (qi::lit("meet")[_c = false]
            | qi::lit("slice")[_c = true]
            )
          )
        )[_val = phx::bind(&preserveAspectRatio_grammar::make_value, _a, _b, _c)];
  }

private:
  typename preserveAspectRatio_grammar::start_type rule_; 

  static preserveAspectRatio_value make_value(bool defer, preserveAspectRatio_value::align_type align, bool slice)
  {
    preserveAspectRatio_value val = {defer, align, slice};
    return val;
  }
};

}