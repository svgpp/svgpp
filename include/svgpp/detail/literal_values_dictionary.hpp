// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#if !BOOST_PP_IS_ITERATING

#ifndef FILE_0C568232_06D8_4754_93E0_95F1E170E5B5_H_
#define FILE_0C568232_06D8_4754_93E0_95F1E170E5B5_H_

#include <boost/preprocessor.hpp>
#include <boost/range/iterator_range.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/detail/literal_char_types.hpp>

namespace svgpp { namespace detail
{

template<class Ch>
struct property_values_dictionary
{
  typedef Ch char_type;
  typedef boost::iterator_range<char_type const *> string_type;
};

template<class Ch> 
struct literal_values_dictionary;

#define BOOST_PP_FILENAME_1 <svgpp/detail/literal_values_dictionary.hpp>
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(SVGPP_CHAR_TYPES)))
#include BOOST_PP_ITERATE()

}}

#endif // FILE_0C568232_06D8_4754_93E0_95F1E170E5B5_H_

#else // !BOOST_PP_IS_ITERATING

#define SVGPP_ITER_CHAR_TYPE \
  BOOST_PP_TUPLE_ELEM(2, 0, BOOST_PP_SEQ_ELEM(BOOST_PP_ITERATION(), SVGPP_CHAR_TYPES))

#define SVGPP_ITER_STRINGIZE \
  BOOST_PP_TUPLE_ELEM(2, 1, BOOST_PP_SEQ_ELEM(BOOST_PP_ITERATION(), SVGPP_CHAR_TYPES))

template<> 
struct literal_values_dictionary<SVGPP_ITER_CHAR_TYPE>
  : property_values_dictionary<SVGPP_ITER_CHAR_TYPE> 
{ 
  template<class ValueTag>
  static string_type get_name();
};

#define SVGPP_ON_VALUE(name) SVGPP_ON_VALUE2(name, name)
#define SVGPP_ON_VALUE2(name, string) \
  template<> \
  inline literal_values_dictionary<SVGPP_ITER_CHAR_TYPE>::string_type \
    literal_values_dictionary<SVGPP_ITER_CHAR_TYPE>::get_name<tag::value::name>() \
  { static const SVGPP_ITER_CHAR_TYPE value[] = BOOST_PP_CAT(SVGPP_ITER_STRINGIZE, #string); \
    return string_type(value, value + sizeof(value)/sizeof(value[0]) - 1); }
#include <svgpp/detail/dict/enumerate_literal_values.inc>
#undef SVGPP_ON_VALUE
#undef SVGPP_ON_VALUE2

#undef SVGPP_ITER_CHAR_TYPE
#undef SVGPP_ITER_STRINGIZE

#endif // !BOOST_PP_IS_ITERATING