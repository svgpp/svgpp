// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/parser/grammar/color.hpp>
#include <svgpp/parser/grammar/icc_color.hpp>

namespace svgpp 
{

namespace qi = boost::spirit::qi;

template <
  class PropertySource, 
  class Iterator, 
  class ColorFactory,
  class ICCColorFactory
>
class color_optional_icc_color_grammar:
  public qi::grammar<Iterator, 
    boost::tuple<typename ColorFactory::color_type, boost::optional<typename ICCColorFactory::icc_color_type> >(ICCColorFactory const &) >
{
  typedef color_optional_icc_color_grammar<PropertySource, Iterator, ColorFactory, ICCColorFactory> this_type;
public:
  color_optional_icc_color_grammar()
    : this_type::grammar(rule_)
  {
    using detail::character_encoding_namespace::space;

    rule_ = 
        color_rule_
        >> - ( +space 
                >> icc_color_rule_(qi::_r1)
             );
  }

private:
  color_grammar<PropertySource, Iterator, ColorFactory> color_rule_;
  icc_color_grammar<PropertySource, Iterator, ICCColorFactory> icc_color_rule_;
  typename this_type::start_type rule_; 
};

}