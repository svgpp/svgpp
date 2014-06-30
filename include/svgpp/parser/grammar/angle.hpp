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
  class Number = double>
class angle_grammar;

template <class Iterator, class AngleFactory, class Number>
class angle_grammar<tag::source::attribute, Iterator, AngleFactory, Number>:
  public qi::grammar<Iterator, typename AngleFactory::angle_type(), qi::locals<Number> >
{
  typedef angle_grammar<tag::source::attribute, Iterator, AngleFactory, Number> this_type;
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
            >>  ( lit("deg")
                      [_val = phx::bind(&angle_grammar::call_make_angle<tag::angle_units::deg>, this, _a)]
                | lit("grad")
                      [_val = phx::bind(&angle_grammar::call_make_angle<tag::angle_units::grad>, this, _a)]
                | lit("rad")
                      [_val = phx::bind(&angle_grammar::call_make_angle<tag::angle_units::rad>, this, _a)]
                );
  }

private:
  typename this_type::start_type rule_; 
  qi::real_parser<Number, detail::number_policies<Number, tag::source::attribute> > number_;

  template<class UnitsTag>
  static angle_type call_make_angle(Number value) 
  {
    return AngleFactory::create(value, UnitsTag());
  }
};

}