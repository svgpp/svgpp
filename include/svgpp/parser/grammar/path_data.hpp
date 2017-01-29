// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/number_type.hpp>
#include <svgpp/parser/detail/common.hpp>
#include <svgpp/policy/path_events.hpp>

namespace svgpp 
{ 

namespace qi = boost::spirit::qi;                         

template <
  class Iterator, 
  class Context, 
  class Coordinate = typename number_type_by_context<Context>::type,
  class EventsPolicy = policy::path_events::default_policy<Context> 
>
class path_data_grammar: public qi::grammar<Iterator, void (Context &), detail::character_encoding_namespace::space_type>
{
  typedef path_data_grammar<Iterator, Context, Coordinate, EventsPolicy> this_type;
public:
  path_data_grammar()
    : this_type::base_type(path)
  {
    namespace phx = boost::phoenix;
    using qi::_1;
    using qi::_2;
    using qi::_3;
    using qi::_4;
    using qi::_5;
    using qi::_6;
    using qi::_7;
    using qi::_a;
    using qi::_r1;
    using qi::_r2;
    using qi::_val;
    using qi::lit;
    using qi::char_;

    path = *(moveto(_r1) >> 
        *(closepath(_r1)
        | lineto(_r1)
        | horizontal_lineto(_r1)
        | vertical_lineto(_r1)
        | curveto(_r1)
        | smooth_curveto(_r1)
        | quadratic_bezier_curveto(_r1)
        | quadratic_bezier_smooth_curveto(_r1)
        | elliptical_arc(_r1)))
        >> qi::eps [phx::bind(&path_data_grammar::call_exit, _r1)];
    moveto = 
        (lit('M') [_a = true] | lit('m') [_a = false]) 
        >> (coordinate >> -lit(',') >> coordinate)
          [phx::bind(&path_data_grammar::call_move_to, _r1, _1, _2, _a)]
        >> -(-lit(',') >> lineto_argument_sequence(_r1, _a));
    closepath =
        char_("Zz") [phx::bind(&path_data_grammar::call_close_subpath, _r1)];
    lineto =
        (lit('L') [_a = true] | lit('l') [_a = false]) 
        >> lineto_argument_sequence(_r1, _a);
    lineto_argument_sequence = 
        (coordinate >> -lit(',') >> coordinate)
          [phx::bind(&path_data_grammar::call_line_to, _r1, _1, _2, _r2)]
        % -lit(',');
    horizontal_lineto =
        (lit('H') [_a = true] | lit('h') [_a = false]) 
        >> (coordinate
            [phx::bind(&path_data_grammar::call_line_to_ortho, _r1, _1, true, _a)]
          % -lit(','));
    vertical_lineto =
        (lit('V') [_a = true] | lit('v') [_a = false]) 
        >> (coordinate 
            [phx::bind(&path_data_grammar::call_line_to_ortho, _r1, _1, false, _a)]
          % -lit(','));
    curveto =
        (lit('C') [_a = true] | lit('c') [_a = false]) 
        >> (curveto_argument(_r1, _a) % -lit(','));
    curveto_argument = 
        (coordinate >> -lit(',') >> coordinate >> -lit(',') >> coordinate >> -lit(',') >> coordinate
            >> -lit(',') >> coordinate >> -lit(',') >> coordinate)
          [phx::bind(&path_data_grammar::call_cubic_bezier_to, _r1, _1, _2, _3, _4, _5, _6, _r2)];
    smooth_curveto =
        (lit('S') [_a = true] | lit('s') [_a = false]) 
        >> (smooth_curveto_argument(_r1, _a) % -lit(','));
    smooth_curveto_argument = 
        (coordinate >> -lit(',') >> coordinate >> -lit(',') >> coordinate >> -lit(',') >> coordinate)
          [phx::bind(&path_data_grammar::call_cubic_bezier_to_shorthand, _r1, _1, _2, _3, _4, _r2)];
    quadratic_bezier_curveto =
        (lit('Q') [_a = true] | lit('q') [_a = false]) 
        >> (quadratic_bezier_curveto_argument(_r1, _a) % -lit(','));
    quadratic_bezier_curveto_argument = 
        (coordinate >> -lit(',') >> coordinate >> -lit(',') >> coordinate >> -lit(',') >> coordinate)
          [phx::bind(&path_data_grammar::call_quadratic_bezier_to, _r1, _1, _2, _3, _4, _r2)];
    quadratic_bezier_smooth_curveto =
        (lit('T') [_a = true] | lit('t') [_a = false]) 
        >> (quadratic_bezier_smooth_curveto_argument(_r1, _a) % -lit(','));
    quadratic_bezier_smooth_curveto_argument = 
        (coordinate >> -lit(',') >> coordinate)
          [phx::bind(&path_data_grammar::call_quadratic_bezier_to_shorthand, _r1, _1, _2, _r2)];
    elliptical_arc =
        (lit('A') [_a = true] | lit('a') [_a = false]) 
        >> (elliptical_arc_argument(_r1, _a) % -lit(','));
    elliptical_arc_argument = 
        (nonnegative_number >> -lit(',') >> nonnegative_number >> -lit(',') >> coordinate
        >> -lit(',') >> flag >> -lit(',') >> flag >> -lit(',') >> coordinate >> -lit(',') >> coordinate)
          [phx::bind(&path_data_grammar::call_elliptical_arc_to, _r1, _1, _2, _3, _4, _5, _6, _7, _r2)];
    flag =
        lit('0') [_val = false] | lit('1') [_val = true];
  }

private:
  typename this_type::start_type path;
  qi::rule<Iterator, void (Context &), typename this_type::skipper_type> closepath;
  qi::rule<Iterator, void (Context &, bool), typename this_type::skipper_type> 
    lineto_argument_sequence, curveto_argument, smooth_curveto_argument,
    quadratic_bezier_curveto_argument, quadratic_bezier_smooth_curveto_argument,
    elliptical_arc_argument;
  qi::rule<Iterator, void (Context &), qi::locals<bool>, typename this_type::skipper_type> 
    moveto, lineto, horizontal_lineto, vertical_lineto, curveto, smooth_curveto,
    quadratic_bezier_curveto, quadratic_bezier_smooth_curveto, elliptical_arc;
  qi::rule<Iterator, bool ()> flag;

  struct nonnegative_real_policies : detail::svg_real_policies<Coordinate>
  {
    inline static bool parse_sign(Iterator&, Iterator const&) { return false; }
  };

  qi::real_parser<Coordinate, detail::svg_real_policies<Coordinate> > coordinate;
  qi::real_parser<Coordinate, nonnegative_real_policies> nonnegative_number;

  static void call_move_to(Context & context, Coordinate x, Coordinate y, bool absolute)
  { 
    if (absolute)
      EventsPolicy::path_move_to(context, x, y, tag::coordinate::absolute()); 
    else
      EventsPolicy::path_move_to(context, x, y, tag::coordinate::relative()); 
  }

  static void call_line_to(Context & context, Coordinate x, Coordinate y, bool absolute)
  { 
    if (absolute)
      EventsPolicy::path_line_to(context, x, y, tag::coordinate::absolute()); 
    else
      EventsPolicy::path_line_to(context, x, y, tag::coordinate::relative()); 
  }

  static void call_line_to_ortho(Context & context, Coordinate coord, bool horizontal, bool absolute)
  { 
    if (absolute)
      EventsPolicy::path_line_to_ortho(context, coord, horizontal, tag::coordinate::absolute()); 
    else
      EventsPolicy::path_line_to_ortho(context, coord, horizontal, tag::coordinate::relative()); 
  }

  static void call_cubic_bezier_to(Context & context, Coordinate x1, Coordinate y1, 
                                        Coordinate x2, Coordinate y2, 
                                        Coordinate x, Coordinate y, 
                                        bool absolute)
  { 
    if (absolute)
      EventsPolicy::path_cubic_bezier_to(context, x1, y1, x2, y2, x, y, tag::coordinate::absolute()); 
    else
      EventsPolicy::path_cubic_bezier_to(context, x1, y1, x2, y2, x, y, tag::coordinate::relative()); 
  }

  static void call_cubic_bezier_to_shorthand(Context & context, 
                                        Coordinate x2, Coordinate y2, 
                                        Coordinate x, Coordinate y, 
                                        bool absolute)
  { 
    if (absolute)
      EventsPolicy::path_cubic_bezier_to(context, x2, y2, x, y, tag::coordinate::absolute()); 
    else
      EventsPolicy::path_cubic_bezier_to(context, x2, y2, x, y, tag::coordinate::relative()); 
  }

  static void call_quadratic_bezier_to(Context & context, 
                                        Coordinate x1, Coordinate y1, 
                                        Coordinate x, Coordinate y, 
                                        bool absolute)
  { 
    if (absolute)
      EventsPolicy::path_quadratic_bezier_to(context, x1, y1, x, y, tag::coordinate::absolute()); 
    else
      EventsPolicy::path_quadratic_bezier_to(context, x1, y1, x, y, tag::coordinate::relative()); 
  }

  static void call_quadratic_bezier_to_shorthand(Context & context,
                                        Coordinate x, Coordinate y, 
                                        bool absolute)
  { 
    if (absolute)
      EventsPolicy::path_quadratic_bezier_to(context, x, y, tag::coordinate::absolute()); 
    else
      EventsPolicy::path_quadratic_bezier_to(context, x, y, tag::coordinate::relative()); 
  }

  static void call_elliptical_arc_to(Context & context, 
                                        Coordinate rx, Coordinate ry, Coordinate x_axis_rotation,
                                        bool large_arc_flag, bool sweep_flag, 
                                        Coordinate x, Coordinate y,
                                        bool absolute)
  { 
    if (absolute)
      EventsPolicy::path_elliptical_arc_to(context, rx, ry, x_axis_rotation, large_arc_flag, sweep_flag, x, y, tag::coordinate::absolute()); 
    else
      EventsPolicy::path_elliptical_arc_to(context, rx, ry, x_axis_rotation, large_arc_flag, sweep_flag, x, y, tag::coordinate::relative()); 
  }

  static void call_close_subpath(Context & context)
  { 
    EventsPolicy::path_close_subpath(context); 
  }

  static void call_exit(Context & context)
  { 
    EventsPolicy::path_exit(context); 
  }
};

}