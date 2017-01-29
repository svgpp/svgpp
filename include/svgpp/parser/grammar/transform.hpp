// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/array.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <svgpp/parser/detail/common.hpp>
#include <svgpp/policy/transform_events.hpp>

namespace svgpp 
{ 

namespace qi = boost::spirit::qi;

template <
  class Iterator, 
  class Context, 
  class Number = double, 
  class EventsPolicy = policy::transform_events::default_policy<Context> >
class transform_grammar: public qi::grammar<Iterator, void (Context &), detail::character_encoding_namespace::space_type>
{
  typedef transform_grammar<Iterator, Context, Number, EventsPolicy> this_type;
public:
  transform_grammar()
    : this_type::base_type(transforms)
  {
    namespace phx = boost::phoenix;
    using qi::_1;
    using qi::_2;
    using qi::_3;
    using qi::_4;
    using qi::_5;
    using qi::_6;
    using qi::_a;
    using qi::_b;
    using qi::_c;
    using qi::_r1;
    using qi::lit;
    using qi::no_skip;

    transforms 
        =  -( ( matrix(_r1)
              | translate(_r1)
              | scale(_r1)
              | rotate(_r1)
              | skewX(_r1)
              | skewY(_r1)
              )
              % 
#ifdef SVGPP_STRICT_TRANSFORM_SEPARATOR
                (+no_skip[comma_wsp])
#else
                (*lit(','))
#endif
            );

    matrix 
        =   ( lit("matrix") 
              >> lit('(') 
              >> number >> no_skip[comma_wsp]
              >> number >> no_skip[comma_wsp]
              >> number >> no_skip[comma_wsp]
              >> number >> no_skip[comma_wsp]
              >> number >> no_skip[comma_wsp]
              >> number >> lit(')')
            ) [phx::bind(&call_matrix, _r1, _1, _2, _3, _4, _5, _6)];
    translate 
        =   lit("translate") 
            >> lit('(') 
            >> number [_a = _1] 
            >> ( ( no_skip[comma_wsp] 
                  >> number [_b = _1] 
                  >> lit(')') [phx::bind(&transform_grammar::call_translate, _r1, _a, _b)] 
                )
                | lit(')') [phx::bind(&transform_grammar::call_translate_x, _r1, _a)]
              );
    scale 
        =   lit("scale") 
            >> lit('(') 
            >> number [_a = _1]
            >> ( ( no_skip[comma_wsp] 
                  >> number [_b = _1] 
                  >> lit(')') [phx::bind(&transform_grammar::call_scale, _r1, _a, _b)] 
                )
                | lit(')') [phx::bind(&transform_grammar::call_scale_uniform, _r1, _a)]
              );
    rotate 
        =   lit("rotate") 
            >> lit('(') 
            >> number[_a = _1]
            >> ( ( no_skip[comma_wsp] 
                  >> number [_b = _1] 
                  >> no_skip[comma_wsp] 
                  >> number [_c = _1] 
                  >> lit(')') [phx::bind(&transform_grammar::call_rotate_about_point, _r1, _a, _b, _c)]
                )
                | lit(')') [phx::bind(&transform_grammar::call_rotate, _r1, _a)] 
              );
    skewX 
        =   ( lit("skewX") 
              >> lit('(') 
              >> number 
              >> lit(')') ) [phx::bind(&transform_grammar::call_skew_x, _r1, _1)];
    skewY 
        =   ( lit("skewY") 
              >> lit('(') 
              >> number 
              >> lit(')') ) [phx::bind(&transform_grammar::call_skew_y, _r1, _1)];
  }

private:
  typename this_type::start_type transforms;
  qi::rule<Iterator, void (Context &), typename this_type::skipper_type> 
    matrix, skewX, skewY;
  qi::rule<Iterator, void (Context &), qi::locals<Number, Number>, typename this_type::skipper_type> 
    translate, scale;
  qi::rule<Iterator, void (Context &), qi::locals<Number, Number, Number>, typename this_type::skipper_type> 
    rotate;
  detail::comma_wsp_rule<Iterator> comma_wsp;
  qi::real_parser<Number, detail::svg_real_policies<Number> > number;

  static void call_matrix(Context & context, Number a, Number b, Number c, Number d, Number e, Number f)
  {
    boost::array<Number, 6> const matrix = {{a, b, c, d, e, f}};
    EventsPolicy::transform_matrix(context, matrix);
  }

  static void call_translate(Context & context, Number tx, Number ty)
  {
    EventsPolicy::transform_translate(context, tx, ty);
  }

  static void call_translate_x(Context & context, Number tx)
  {
    EventsPolicy::transform_translate(context, tx);
  }

  static void call_scale(Context & context, Number sx, Number sy)
  {
    EventsPolicy::transform_scale(context, sx, sy);
  }

  static void call_scale_uniform(Context & context, Number scale)
  {
    EventsPolicy::transform_scale(context, scale);
  }

  static void call_rotate(Context & context, Number angle)
  {
    EventsPolicy::transform_rotate(context, angle);
  }

  static void call_rotate_about_point(Context & context, Number angle, Number cx, Number cy)
  {
    EventsPolicy::transform_rotate(context, angle, cx, cy);
  }

  static void call_skew_x(Context & context, Number angle)
  {
    EventsPolicy::transform_skew_x(context, angle);
  }

  static void call_skew_y(Context & context, Number angle)
  {
    EventsPolicy::transform_skew_y(context, angle);
  }
};

}