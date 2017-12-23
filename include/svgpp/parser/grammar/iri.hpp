// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/spirit/include/qi.hpp>
#include <svgpp/parser/detail/common.hpp>

namespace svgpp
{

namespace qi = boost::spirit::qi;

template <class Iterator>
class iri_grammar:
  public qi::grammar<Iterator, typename boost::iterator_range<Iterator>()>
{
  typedef iri_grammar<Iterator> this_type;
public:
  iri_grammar()
    : this_type::grammar(rule_)
  {
    using detail::character_encoding_namespace::char_;

    // TODO: More thorough RFC 3987 check
    rule_
        =
#ifdef SVGPP_ACCEPT_QUOTED_IRI
            (qi::lit('"')
            >> qi::raw[ + (!(char_(')') | char_('"')) >> char_) ]
            >> qi::lit('"' ))
            |
#endif
            qi::raw[ + (!char_(')') >> char_) ];
  }

private:
  typename this_type::start_type rule_; 
};

template <class PropertySource, class Iterator>
class funciri_grammar:
  public qi::grammar<Iterator, typename boost::iterator_range<Iterator>()>
{
  typedef funciri_grammar<PropertySource, Iterator> this_type;
public:
  funciri_grammar()
    : this_type::grammar(rule_)
  {
    using detail::character_encoding_namespace::char_;

    rule_ 
        =   detail::no_case_if_css(PropertySource())[ qi::lit("url(") ] 
            >> iri_ 
            >> qi::lit(')');
  }

private:
  typename this_type::start_type rule_; 
  iri_grammar<Iterator> iri_;
};

}