// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/detail/scoped_enum_emulation.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/range/iterator_range.hpp>

#define SVGPP_SVG_NAMESPACE_URI "http://www.w3.org/2000/svg"
#define SVGPP_XML_NAMESPACE_URI "http://www.w3.org/XML/1998/namespace"
#define SVGPP_XLINK_NAMESPACE_URI "http://www.w3.org/1999/xlink"

namespace svgpp { namespace detail 
{

BOOST_SCOPED_ENUM_START(namespace_id) { svg, xml, xlink, other }; BOOST_SCOPED_ENUM_END

template<class Ch> inline boost::iterator_range<Ch const *> svg_namespace_uri();
template<class Ch> inline boost::iterator_range<Ch const *> xml_namespace_uri();
template<class Ch> inline boost::iterator_range<Ch const *> xlink_namespace_uri();

template<>
inline boost::iterator_range<char const *> svg_namespace_uri<char>()
{
  static const char value[] = SVGPP_SVG_NAMESPACE_URI;
  return boost::iterator_range<char const *>(value, boost::end(value) - 1);
}

template<>
inline boost::iterator_range<wchar_t const *> svg_namespace_uri<wchar_t>()
{
  static const wchar_t value[] = BOOST_PP_CAT(L, SVGPP_SVG_NAMESPACE_URI);
  return boost::iterator_range<wchar_t const *>(value, boost::end(value) - 1);
}

template<>
inline boost::iterator_range<char const *> xml_namespace_uri<char>()
{
  static const char value[] = SVGPP_XML_NAMESPACE_URI;
  return boost::iterator_range<char const *>(value, boost::end(value) - 1);
}

template<>
inline boost::iterator_range<wchar_t const *> xml_namespace_uri<wchar_t>()
{
  static const wchar_t value[] = BOOST_PP_CAT(L, SVGPP_XML_NAMESPACE_URI);
  return boost::iterator_range<wchar_t const *>(value, boost::end(value) - 1);
}

template<>
inline boost::iterator_range<char const *> xlink_namespace_uri<char>()
{
  static const char value[] = SVGPP_XLINK_NAMESPACE_URI;
  return boost::iterator_range<char const *>(value, boost::end(value) - 1);
}

template<>
inline boost::iterator_range<wchar_t const *> xlink_namespace_uri<wchar_t>()
{
  static const wchar_t value[] = BOOST_PP_CAT(L, SVGPP_XLINK_NAMESPACE_URI);
  return boost::iterator_range<wchar_t const *>(value, boost::end(value) - 1);
}

}}