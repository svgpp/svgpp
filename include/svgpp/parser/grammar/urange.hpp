// Copyright Oleg Maximenko 2014.
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

template <class Iterator, class Number = unsigned int>
class urange_grammar:
  public qi::grammar<Iterator, std::pair<Number, Number>(), qi::locals<Number> >
{
public:
  urange_grammar()
    : urange_grammar::grammar(rule_)
  {
    using qi::_a;
    using qi::_1;
    using qi::_val;
    using qi::lit;
    using qi::hex;
    using qi::raw;
    namespace phx = boost::phoenix;

    rule_ 
        =   lit("U+") >> hex[_a = _1]
            >> ( raw[ + lit('?') ]
                    [_val = phx::bind(&urange_grammar::make_mask_range, _a, _1)] 
               | ( lit('-') 
                   >> hex [_val = phx::bind(&urange_grammar::make_range, _a, _1)] 
                 )
               | qi::eps [_val = phx::bind(&urange_grammar::make_range, _a, _a)]
               );
  }

private:
  typename urange_grammar::start_type rule_; 

  static std::pair<Number, Number> make_range(Number start, Number end)
  {
    return std::pair<Number, Number>(start, end);
  }

  static std::pair<Number, Number> make_mask_range(Number base, boost::iterator_range<Iterator> const & mask)
  {
    Number mask_mul = 1 << (boost::size(mask) * 4);

    return std::pair<Number, Number>(base * mask_mul, (base + 1) * mask_mul - 1);
  }
};

}