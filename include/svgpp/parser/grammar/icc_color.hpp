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

namespace svgpp 
{

namespace qi = boost::spirit::qi;

template <
  class PropertySource, 
  class Iterator, 
  class ICCColorFactory, 
  class Number = typename ICCColorFactory::component_type
>
class icc_color_grammar:
  public qi::grammar<Iterator, typename ICCColorFactory::icc_color_type(ICCColorFactory const &), qi::locals<typename ICCColorFactory::builder_type> >
{
  typedef icc_color_grammar<PropertySource, Iterator, ICCColorFactory, Number> this_type;
public:
  icc_color_grammar()
    : this_type::grammar(rule_)
  {
    namespace phx = boost::phoenix;
    using detail::character_encoding_namespace::char_;

    rule_ 
        = detail::no_case_if_css(PropertySource())[ qi::lit("icc-color(") ]
          >> qi::raw[ +(!char_(",() \r\n\t") >> char_) ]  // [^,()#x20#x9#xD#xA]  any char except ",", "(", ")" or wsp 
                [phx::bind(&icc_color_grammar::call_set_profile_name, qi::_r1, qi::_a, qi::_1)]
          >> + ( 
                comma_wsp_ 
                >> number_ [phx::bind(&icc_color_grammar::call_append_component_value, qi::_r1, qi::_a, qi::_1)]
               )
          >> qi::lit(")")
                [qi::_val = phx::bind(&icc_color_grammar::call_create_icc_color, qi::_r1, qi::_a)];
  }

private:
  typename this_type::start_type rule_; 
  detail::comma_wsp_rule_no_skip<Iterator> comma_wsp_;
  qi::real_parser<Number, detail::number_policies<Number, PropertySource> > number_;

  static void call_set_profile_name(ICCColorFactory const & color_factory, 
    typename ICCColorFactory::builder_type & builder, boost::iterator_range<Iterator> const & profile_name)
  {
    color_factory.set_profile_name(builder, profile_name);
  }

  static void call_append_component_value(ICCColorFactory const & color_factory, 
    typename ICCColorFactory::builder_type & builder, Number value)
  {
    color_factory.append_component_value(builder, value);
  }

  static typename ICCColorFactory::icc_color_type 
    call_create_icc_color(ICCColorFactory const & color_factory, typename ICCColorFactory::builder_type & builder)
  {
    return color_factory.create_icc_color(builder);
  }
};

}