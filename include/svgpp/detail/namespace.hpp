// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#if !BOOST_PP_IS_ITERATING

#ifndef FILE_AF316121_84B7_4A69_BBE7_EEEADDABD0FF_H_
#define FILE_AF316121_84B7_4A69_BBE7_EEEADDABD0FF_H_

#include <boost/detail/scoped_enum_emulation.hpp>
#include <boost/preprocessor.hpp>
#include <boost/range/iterator_range.hpp>
#include <svgpp/detail/literal_char_types.hpp>

#define SVGPP_SVG_NAMESPACE_URI "http://www.w3.org/2000/svg"
#define SVGPP_XML_NAMESPACE_URI "http://www.w3.org/XML/1998/namespace"
#define SVGPP_XLINK_NAMESPACE_URI "http://www.w3.org/1999/xlink"

namespace svgpp { namespace detail 
{

BOOST_SCOPED_ENUM_START(namespace_id) { svg, xml, xlink, other }; BOOST_SCOPED_ENUM_END

template<class Ch> inline boost::iterator_range<Ch const *> svg_namespace_uri();
template<class Ch> inline boost::iterator_range<Ch const *> xml_namespace_uri();
template<class Ch> inline boost::iterator_range<Ch const *> xlink_namespace_uri();

#define BOOST_PP_FILENAME_1 <svgpp/detail/namespace.hpp>
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(SVGPP_CHAR_TYPES)))
#include BOOST_PP_ITERATE()

}}

#endif // FILE_AF316121_84B7_4A69_BBE7_EEEADDABD0FF_H_

#else // !BOOST_PP_IS_ITERATING

#define SVGPP_ITER_CHAR_TYPE \
  BOOST_PP_TUPLE_ELEM(2, 0, BOOST_PP_SEQ_ELEM(BOOST_PP_ITERATION(), SVGPP_CHAR_TYPES))

#define SVGPP_ITER_STRINGIZE \
  BOOST_PP_TUPLE_ELEM(2, 1, BOOST_PP_SEQ_ELEM(BOOST_PP_ITERATION(), SVGPP_CHAR_TYPES))


template<>
inline boost::iterator_range<SVGPP_ITER_CHAR_TYPE const *> svg_namespace_uri<SVGPP_ITER_CHAR_TYPE>()
{
  static const SVGPP_ITER_CHAR_TYPE value[] = BOOST_PP_CAT(SVGPP_ITER_STRINGIZE, SVGPP_SVG_NAMESPACE_URI);
  return boost::iterator_range<SVGPP_ITER_CHAR_TYPE const *>(value, boost::end(value) - 1);
}

template<>
inline boost::iterator_range<SVGPP_ITER_CHAR_TYPE const *> xml_namespace_uri<SVGPP_ITER_CHAR_TYPE>()
{
  static const SVGPP_ITER_CHAR_TYPE value[] = BOOST_PP_CAT(SVGPP_ITER_STRINGIZE, SVGPP_XML_NAMESPACE_URI);
  return boost::iterator_range<SVGPP_ITER_CHAR_TYPE const *>(value, boost::end(value) - 1);
}

template<>
inline boost::iterator_range<SVGPP_ITER_CHAR_TYPE const *> xlink_namespace_uri<SVGPP_ITER_CHAR_TYPE>()
{
  static const SVGPP_ITER_CHAR_TYPE value[] = BOOST_PP_CAT(SVGPP_ITER_STRINGIZE, SVGPP_XLINK_NAMESPACE_URI);
  return boost::iterator_range<SVGPP_ITER_CHAR_TYPE const *>(value, boost::end(value) - 1);
}

#undef SVGPP_ITER_CHAR_TYPE
#undef SVGPP_ITER_STRINGIZE

#endif // !BOOST_PP_IS_ITERATING
