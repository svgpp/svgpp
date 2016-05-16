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

namespace detail
{
  struct length_units_symbols
  {
    enum type {em, ex, px, in, cm, mm, pt, pc};

    length_units_symbols()
    {
      symbols_.add("em", em);
      symbols_.add("ex", ex);
      symbols_.add("px", px);
      symbols_.add("in", in);
      symbols_.add("cm", cm);
      symbols_.add("mm", mm);
      symbols_.add("pt", pt);
      symbols_.add("pc", pc);
    }

    qi::symbols<char, type> const & symbols() const { return symbols_; }

    template<class LengthFactory, class Value>
    static typename LengthFactory::length_type create_length(LengthFactory const & length_factory, Value value, type units)
    {
      switch (units)
      {
      default:
        BOOST_ASSERT(false);
      case em:
        return length_factory.create_length(value, tag::length_units::em());
      case ex:
        return length_factory.create_length(value, tag::length_units::ex());
      case px:
        return length_factory.create_length(value, tag::length_units::px());
      case in:
        return length_factory.create_length(value, tag::length_units::in());
      case cm:
        return length_factory.create_length(value, tag::length_units::cm());
      case mm:
        return length_factory.create_length(value, tag::length_units::mm());
      case pt:
        return length_factory.create_length(value, tag::length_units::pt());
      case pc:
        return length_factory.create_length(value, tag::length_units::pc());
      }
    }

  private:
    qi::symbols<char, type> symbols_;
  };

  template<class LengthFactory, class Value>
  inline typename LengthFactory::length_type call_make_length_without_units(LengthFactory const & length_factory, Value value)
  {
    return length_factory.create_length(value, tag::length_units::none());
  }
}

template <
  class PropertySource, 
  class Iterator, 
  class LengthFactory, 
  class PercentageDirectionTag = tag::length_dimension::not_width_nor_height,
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
            >>  ( units_symbols_.symbols()
                      [_val = phx::bind(&detail::length_units_symbols::create_length<LengthFactory, Number>, _r1, _a, _1)]
                | qi::lit("%")
                      [_val = phx::bind(&length_grammar::call_make_length_percent, _r1, _a)]
                | qi::eps
                      [_val = phx::bind(&detail::call_make_length_without_units<LengthFactory, Number>, _r1, _a)]
                );
  }

private:
  typename this_type::start_type rule_; 
  qi::real_parser<Number, detail::number_policies<Number, tag::source::attribute> > number_;
  detail::length_units_symbols units_symbols_;

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
            >>  ( qi::no_case
                  [
                    units_symbols_.symbols()
                        [_val = phx::bind(&detail::length_units_symbols::create_length<LengthFactory, Number>, _r1, _a, _1)]
                  ]
                | qi::eps
                      [_val = phx::bind(&detail::call_make_length_without_units<LengthFactory, Number>, _r1, _a)]
                );
  }

private:
  typename this_type::start_type rule_; 
  qi::real_parser<Number, detail::number_policies<Number, tag::source::css> > number_;
  detail::length_units_symbols units_symbols_;
};

template <
  class Iterator, 
  class LengthFactory, 
  class Number = typename LengthFactory::number_type
>
class percentage_or_length_css_grammar:
  public qi::grammar<Iterator, typename LengthFactory::length_type(LengthFactory const &), qi::locals<Number> >
{
  typedef percentage_or_length_css_grammar<Iterator, LengthFactory, Number> this_type;
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
            >>  ( qi::no_case
                  [
                    units_symbols_.symbols()
                        [_val = phx::bind(&detail::length_units_symbols::create_length<LengthFactory, Number>, _r1, _a, _1)]
                  ]
                | qi::lit("%")
                      [_val = phx::bind(&this_type::call_make_length_percent, _r1, _a)]
                | qi::eps
                      [_val = phx::bind(&detail::call_make_length_without_units<LengthFactory, Number>, _r1, _a)]
                );
  }

private:
  typename this_type::start_type rule_; 
  qi::real_parser<Number, detail::number_policies<Number, tag::source::css> > number_;
  detail::length_units_symbols units_symbols_;

  static length_type call_make_length_percent(LengthFactory const & length_factory, Number value)
  {
    return length_factory.create_length(value, tag::length_units::percent(), tag::length_dimension::not_width_nor_height());
  }
};

}