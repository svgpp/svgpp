#pragma once

#include <boost/array.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <svgpp/parser/detail/common.hpp>
#include <svgpp/policy/load_transform.hpp>

namespace svgpp 
{ 

namespace qi = boost::spirit::qi;

template <
  class Iterator, 
  class Context, 
  class Number = double, 
  class LoadPolicy = policy::load_transform::default_policy<Context> >
class transform_grammar: public qi::grammar<Iterator, void (Context &), detail::character_encoding_namespace::space_type>
{
  typedef transform_grammar<Iterator, Context, Number, LoadPolicy> this_type;
public:
  transform_grammar()
    : this_type::base_type(transforms)
  {
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
              % (+no_skip[comma_wsp]));

    matrix 
        =   ( lit("matrix") 
              >> lit('(') 
              >> number >> no_skip[comma_wsp]
              >> number >> no_skip[comma_wsp]
              >> number >> no_skip[comma_wsp]
              >> number >> no_skip[comma_wsp]
              >> number >> no_skip[comma_wsp]
              >> number >> lit(')')
            ) [bind(&call_matrix, _r1, _1, _2, _3, _4, _5, _6)];
    translate 
        =   lit("translate") 
            >> lit('(') 
            >> number [_a = _1] 
            >> ( ( no_skip[comma_wsp] 
                  >> number [_b = _1] 
                  >> lit(')') [bind(&call_translate, _r1, _a, _b)] 
                )
                | lit(')') [bind(&call_translate, _r1, _a)] 
              );
    scale 
        =   lit("scale") 
            >> lit('(') 
            >> number [_a = _1]
            >> ( ( no_skip[comma_wsp] 
                  >> number [_b = _1] 
                  >> lit(')') [bind(&call_scale, _r1, _a, _b)] 
                )
                | lit(')') [bind(&call_scale, _r1, _a)] 
              );
    rotate 
        =   lit("rotate") 
            >> lit('(') 
            >> number[_a = _1]
            >> ( ( no_skip[comma_wsp] 
                  >> number [_b = _1] 
                  >> no_skip[comma_wsp] 
                  >> number [_c = _1] 
                  >> lit(')') [bind(&call_rotate, _r1, _a, _b, _c)] 
                )
                | lit(')') [bind(&call_rotate, _r1, _a)] 
              );
    skewX 
        =   ( lit("skewX") 
              >> lit('(') 
              >> number 
              >> lit(')') ) [bind(&call_skew_x, _r1, _1)];
    skewY 
        =   ( lit("skewY") 
              >> lit('(') 
              >> number 
              >> lit(')') ) [bind(&call_skew_y, _r1, _1)];
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
    LoadPolicy::append_transform_matrix(context, matrix);
  }

  static void call_translate(Context & context, Number tx, Number ty)
  {
    LoadPolicy::append_transform_translate(context, tx, ty);
  }

  static void call_translate(Context & context, Number tx)
  {
    LoadPolicy::append_transform_translate(context, tx);
  }

  static void call_scale(Context & context, Number sx, Number sy)
  {
    LoadPolicy::append_transform_scale(context, sx, sy);
  }

  static void call_scale(Context & context, Number scale)
  {
    LoadPolicy::append_transform_scale(context, scale);
  }

  static void call_rotate(Context & context, Number angle)
  {
    LoadPolicy::append_transform_rotate(context, angle);
  }

  static void call_rotate(Context & context, Number angle, Number cx, Number cy)
  {
    LoadPolicy::append_transform_rotate(context, angle, cx, cy);
  }

  static void call_skew_x(Context & context, Number angle)
  {
    LoadPolicy::append_transform_skew_x(context, angle);
  }

  static void call_skew_y(Context & context, Number angle)
  {
    LoadPolicy::append_transform_skew_y(context, angle);
  }
};

}