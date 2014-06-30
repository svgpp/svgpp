#pragma once

#include <boost/preprocessor/cat.hpp>
#include <boost/range/iterator_range.hpp>

#define SVGPP_SVG_NAMESPACE_URI "http://www.w3.org/2000/svg"
#define SVGPP_XML_NAMESPACE_URI "http://www.w3.org/XML/1998/namespace"
#define SVGPP_XLINK_NAMESPACE_URI "http://www.w3.org/1999/xlink"

namespace svgpp { namespace detail 
{

enum class namespace_id { svg, xml, xlink, other };

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