#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <svgpp/parser/detail/common.hpp>

namespace svgpp { 

template<class Context>
class load_list_of_points_policy
{
public:
  template<class Coordinate>
  static void append_list_point(Context & context, Coordinate x, Coordinate y)
  {
    context.append_list_point(x, y);
  }
};
  
namespace qi = boost::spirit::qi;

template <
  class Iterator, 
  class Context, 
  class Coordinate = double,
  class LoadPolicy = load_list_of_points_policy<Context> >
class list_of_points_grammar: 
  public qi::grammar<Iterator, void (Context &), detail::character_encoding_namespace::space_type>
{
  typedef list_of_points_grammar<Iterator, Context, Coordinate, LoadPolicy> this_type;
public:
  list_of_points_grammar()
    : this_type::base_type(points)
  {
    points = -(
      (number >> -qi::lit(',') >> number) 
        [bind(&call_point, qi::_r1, qi::_1, qi::_2)]
      % qi::no_skip[comma_wsp]);
  }

private:
  typename this_type::start_type points;
  detail::comma_wsp_rule<Iterator> comma_wsp;
  qi::real_parser<Coordinate, detail::svg_real_policies<Coordinate> > number;

  static void call_point(Context & context, Coordinate x, Coordinate y)
  {
    LoadPolicy::append_list_point(context, x, y);
  }
};

}