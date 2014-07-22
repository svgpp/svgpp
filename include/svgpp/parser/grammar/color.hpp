// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/parser/detail/common.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace svgpp 
{

namespace qi = boost::spirit::qi;

namespace detail
{
  typedef boost::tuple<unsigned char, unsigned char, unsigned char> rgb_t;

  namespace
  {
    struct color_keywords
    {
      typedef qi::symbols<char, rgb_t> symbols;
      static bool initialized_;
      static symbols symbols_;
    };

    bool color_keywords::initialized_ = false;
    color_keywords::symbols color_keywords::symbols_;
  }
}

template <class Iterator, class ColorFactory>
class color_grammar: public qi::grammar<Iterator, typename ColorFactory::color_type()>
{
  typedef color_grammar<Iterator, ColorFactory> this_type;
public:
  typedef typename ColorFactory::color_type color_type;

  color_grammar()
    : this_type::grammar(color)
  {
    using qi::_1;
    using qi::_a;
    using qi::_b;
    using qi::_c;
    using qi::_val;
    using qi::lit;
    using detail::character_encoding_namespace::space;

    comma 
        =   *space 
            >> ',' 
            >> *space;
    color 
        =   hex_rule [_val = _1]
        |   components_rule [_val = _1]
        |   detail::color_keywords::symbols_ [_val = bind(&color_keyword, _1)];

    hex_rule 
        =   lit('#') 
            >> hex3 [_a = _1] 
            >> ( hex3 [_val = bind(&six_hex_digits, _a, _1)] 
              | qi::eps [_val = bind(&three_hex_digits, _a)] 
              );

    components_rule 
        =   lit("rgb(") 
            >> *space 
            >> integer [_a = _1] 
            >> ( ( comma 
                  >> integer [_b = _1] 
                  >> comma 
                  >> integer [_c = _1] 
                  >> *space 
                  >> ')' 
                ) [_val = bind(&absolute_components, _a, _b, _c)]
              | ( lit('%') 
                  >> comma 
                  >> integer [_b = _1] 
                  >> '%' 
                  >> comma 
                  >> integer [_c = _1] 
                  >> '%' 
                  >> *space 
                  >> ')' 
                ) [_val = bind(&percent_components, _a, _b, _c)]
              );
  }

private:
  typename this_type::start_type color;
  qi::rule<Iterator> comma;
  qi::rule<Iterator, color_type (), qi::locals<unsigned int> > hex_rule;
  qi::rule<Iterator, color_type (), qi::locals<unsigned char, unsigned char, unsigned char> > components_rule;
  qi::uint_parser< unsigned char, 10, 1, 3 > integer;
  qi::uint_parser< unsigned int, 16, 3, 3 > hex3;

  static color_type three_hex_digits(unsigned int h)
  {
    return ColorFactory::create( 
      static_cast<unsigned char>( ( (h & 0xf00) >> 8 ) | ( (h & 0xf00) >> 4 ) ),
      static_cast<unsigned char>( ( (h & 0x0f0) >> 4 ) |   (h & 0x0f0) ),
      static_cast<unsigned char>( ( (h & 0x00f) << 4 ) |   (h & 0x00f) ) );
  }

  static color_type six_hex_digits(unsigned int h1, unsigned int h2)
  {
    return ColorFactory::create( 
      static_cast<unsigned char>( h1 >> 4 ), 
      static_cast<unsigned char>( ( ( h1 & 0x0f ) << 4 ) | ( h2 >> 8 ) ),
      static_cast<unsigned char>( h2 & 0xff ) );
  }

  static color_type absolute_components(unsigned char r, unsigned char g, unsigned char b)
  {
    return ColorFactory::create(r, g, b);
  }

  static color_type percent_components(unsigned char r, unsigned char g, unsigned char b)
  {
    return ColorFactory::create_from_percent(r, g, b);
  }

  static color_type color_keyword(detail::rgb_t const & rgb)
  {
    return ColorFactory::create(rgb.get<0>(), rgb.get<1>(), rgb.get<2>());
  }
};

namespace detail
{
  namespace
  {
    struct color_keywords_initializer
    {
      color_keywords_initializer()
      {
        if (!color_keywords::initialized_)
        {
          color_keywords::symbols_.add
#define SVGPP_ON(name, r, g, b) (#name, rgb_t(r, g, b))
#include <svgpp/detail/dict/enumerate_colors.inc>
#undef SVGPP_ON
            ;
          color_keywords::initialized_ = true;
        }
      }
    };

    color_keywords_initializer color_keywords_initializer_instance;
  }
}

}