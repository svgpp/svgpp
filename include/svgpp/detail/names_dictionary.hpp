// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#if !BOOST_PP_IS_ITERATING

#ifndef FILE_B3F16CC7_A6B9_4315_9958_A9FE298EC3A7_H_
#define FILE_B3F16CC7_A6B9_4315_9958_A9FE298EC3A7_H_

#include <svgpp/definitions.hpp>
#include <svgpp/detail/literal_char_types.hpp>
#include <algorithm>
#include <string>
#include <boost/static_assert.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/preprocessor.hpp>
#include <boost/preprocessor/wstringize.hpp>
#include <cstring>
#include <cwchar>

namespace svgpp { namespace detail {

template<class MappedType>
struct dictionary_base
{
  typedef MappedType mapped_type;

  template<class Ch> struct value_type
  {
    Ch const * key;
    size_t key_length;
    mapped_type value;
  };
};

struct element_name_to_id: dictionary_base<element_type_id>
{
  template<class Ch>
  inline static boost::iterator_range<value_type<Ch> const *> const & get_map();
};

struct svg_attribute_name_to_id: dictionary_base<attribute_id>
{
  template<class Ch>
  inline static boost::iterator_range<value_type<Ch> const *> const & get_map();
};

struct xlink_attribute_name_to_id: dictionary_base<attribute_id>
{
  template<class Ch>
  inline static boost::iterator_range<value_type<Ch> const *> const & get_map();
};

struct xml_attribute_name_to_id: dictionary_base<attribute_id>
{
  template<class Ch>
  inline static boost::iterator_range<value_type<Ch> const *> const & get_map();
};

struct css_property_name_to_id: dictionary_base<attribute_id>
{
  struct lower_case_values;

  template<class Ch>
  inline static boost::iterator_range<value_type<Ch> const *> const & get_map();
};

template<class ValuesHolder, typename ValuesHolder::mapped_type NotFoundValue>
struct static_dictionary
{
  template<class Range>
  static typename ValuesHolder::mapped_type find(Range const & key)
  {
    return find(boost::as_literal(key), key_less());
  }

  template<class Range>
  static typename ValuesHolder::mapped_type find_ignore_case(Range const & key)
  {
    typedef typename ValuesHolder::lower_case_values check;
    return find(boost::as_literal(key), key_iless());
  }

private:
  template<class Range, class Pred>
  static typename ValuesHolder::mapped_type find(Range const & key, Pred const & pred)
  {
    typedef typename boost::range_const_iterator<Range>::type iterator_type;
    typedef typename std::iterator_traits<iterator_type>::value_type char_type;
    typedef typename ValuesHolder::template value_type<char_type> value_type;
    typedef boost::iterator_range<value_type const *> map_type;

    map_type map = ValuesHolder::template get_map<char_type>();
    typename map_type::const_iterator item = std::lower_bound(boost::begin(map), boost::end(map), key, pred);
    if (item == boost::end(map) || pred(key, *item))
      return NotFoundValue;
    return item->value;
  }

  struct key_less
  {
    template<class Range, class Char>
    bool operator()(
      const Range & lhs,
      typename ValuesHolder::template value_type<Char> const & rhs) const
    {
      return std::lexicographical_compare(
        boost::begin(lhs),
        boost::end(lhs),
        rhs.key,
        rhs.key + rhs.key_length);
    }

    template<class Range, class Char>
    bool operator()(
      typename ValuesHolder::template value_type<Char> const & lhs,
      const Range & rhs) const
    {
      return std::lexicographical_compare(
        lhs.key,
        lhs.key + lhs.key_length,
        boost::begin(rhs),
        boost::end(rhs));
    }
  };

  struct key_iless
  {
  private:
    boost::algorithm::is_iless is_iless_;

  public:
    template<class Range, class Char>
    bool operator()(
      const Range & lhs,
      typename ValuesHolder::template value_type<Char> const & rhs) const
    {
      return std::lexicographical_compare(
        boost::begin(lhs),
        boost::end(lhs),
        rhs.key,
        rhs.key + rhs.key_length,
        is_iless_);
    }

    template<class Range, class Char>
    bool operator()(
      typename ValuesHolder::template value_type<Char> const & lhs,
      const Range & rhs) const
    {
      return std::lexicographical_compare(
        lhs.key,
        lhs.key + lhs.key_length,
        boost::begin(rhs),
        boost::end(rhs),
        is_iless_);
    }
  };
};

#define BOOST_PP_FILENAME_1 <svgpp/detail/names_dictionary.hpp>
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(SVGPP_CHAR_TYPES)))
#include BOOST_PP_ITERATE()

typedef static_dictionary<element_name_to_id, unknown_element_type_id> element_name_to_id_dictionary;
typedef static_dictionary<svg_attribute_name_to_id, unknown_attribute_id> svg_attribute_name_to_id_dictionary;
typedef static_dictionary<xml_attribute_name_to_id, unknown_attribute_id> xml_attribute_name_to_id_dictionary;
typedef static_dictionary<xlink_attribute_name_to_id, unknown_attribute_id> xlink_attribute_name_to_id_dictionary;
typedef static_dictionary<css_property_name_to_id, unknown_attribute_id> css_property_name_to_id_dictionary;

}}

#endif // FILE_B3F16CC7_A6B9_4315_9958_A9FE298EC3A7_H_

#else // !BOOST_PP_IS_ITERATING

#define SVGPP_ITER_CHAR_TYPE \
  BOOST_PP_TUPLE_ELEM(2, 0, BOOST_PP_SEQ_ELEM(BOOST_PP_ITERATION(), SVGPP_CHAR_TYPES))

#define SVGPP_ITER_STRINGIZE \
  BOOST_PP_TUPLE_ELEM(2, 1, BOOST_PP_SEQ_ELEM(BOOST_PP_ITERATION(), SVGPP_CHAR_TYPES))

#define SVGPP_ADD_ITEM(key, value) \
  { BOOST_PP_CAT(SVGPP_ITER_STRINGIZE, #key), sizeof(BOOST_PP_CAT(SVGPP_ITER_STRINGIZE, #key)) / sizeof(SVGPP_ITER_CHAR_TYPE) - 1, value },

template<>
inline boost::iterator_range<element_name_to_id::value_type<SVGPP_ITER_CHAR_TYPE> const *> const & 
  element_name_to_id::get_map<SVGPP_ITER_CHAR_TYPE>()
{
  static value_type<SVGPP_ITER_CHAR_TYPE> const map[] = {
#define SVGPP_ON(name, str) SVGPP_ADD_ITEM(str, element_type_id_## name)
#include <svgpp/detail/dict/enumerate_all_elements.inc>
#undef SVGPP_ON
  };
  BOOST_STATIC_ASSERT(sizeof(map) / sizeof(map[0]) == element_type_count);
  static const boost::iterator_range<element_name_to_id::value_type<SVGPP_ITER_CHAR_TYPE> const *> range 
    = boost::make_iterator_range(map);
  return range;
}

template<>
inline boost::iterator_range<svg_attribute_name_to_id::value_type<SVGPP_ITER_CHAR_TYPE> const *> const & 
  svg_attribute_name_to_id::get_map<SVGPP_ITER_CHAR_TYPE>()
{
  static value_type<SVGPP_ITER_CHAR_TYPE> const map[] = {
#define SVGPP_ON(name, str) SVGPP_ADD_ITEM(str, attribute_id_## name)
#define SVGPP_ON_STYLE(name, str) SVGPP_ON(name, str) 
#define SVGPP_ON_NS(ns, name, str)
#include <svgpp/detail/dict/enumerate_all_attributes.inc>
#undef SVGPP_ON
#undef SVGPP_ON_STYLE
#undef SVGPP_ON_NS
  };
  static const boost::iterator_range<svg_attribute_name_to_id::value_type<SVGPP_ITER_CHAR_TYPE> const *> range 
    = boost::make_iterator_range(map);
  return range;
}

template<>
inline boost::iterator_range<xlink_attribute_name_to_id::value_type<SVGPP_ITER_CHAR_TYPE> const *> const & 
  xlink_attribute_name_to_id::get_map<SVGPP_ITER_CHAR_TYPE>()
{
  static value_type<SVGPP_ITER_CHAR_TYPE> const map[] = {
#define SVGPP_ON(name, str) 
#define SVGPP_ON_STYLE(name, str) SVGPP_ON(name, str) 
#define SVGPP_ON_NS_xlink(name, str) SVGPP_ADD_ITEM(str, attribute_id_xlink_## name)
#define SVGPP_ON_NS_xml(name, str)
#define SVGPP_ON_NS(ns, name, str) BOOST_PP_CAT(SVGPP_ON_NS_, ns)(name, str)
#include <svgpp/detail/dict/enumerate_all_attributes.inc>
#undef SVGPP_ON
#undef SVGPP_ON_STYLE
#undef SVGPP_ON_NS
#undef SVGPP_ON_NS_xlink
#undef SVGPP_ON_NS_xml
  };
  BOOST_STATIC_ASSERT(sizeof(map) / sizeof(map[0]) == 7);
  static const boost::iterator_range<xlink_attribute_name_to_id::value_type<SVGPP_ITER_CHAR_TYPE> const *> range 
    = boost::make_iterator_range(map);
  return range;
}

template<>
inline boost::iterator_range<xml_attribute_name_to_id::value_type<SVGPP_ITER_CHAR_TYPE> const *> const & 
  xml_attribute_name_to_id::get_map<SVGPP_ITER_CHAR_TYPE>()
{
  static value_type<SVGPP_ITER_CHAR_TYPE> const map[] = {
#define SVGPP_ON(name, str) 
#define SVGPP_ON_STYLE(name, str) SVGPP_ON(name, str) 
#define SVGPP_ON_NS_xml(name, str) SVGPP_ADD_ITEM(str, attribute_id_xml_## name)
#define SVGPP_ON_NS_xlink(name, str)
#define SVGPP_ON_NS(ns, name, str) BOOST_PP_CAT(SVGPP_ON_NS_, ns)(name, str)
#include <svgpp/detail/dict/enumerate_all_attributes.inc>
#undef SVGPP_ON
#undef SVGPP_ON_STYLE
#undef SVGPP_ON_NS
#undef SVGPP_ON_NS_xlink
#undef SVGPP_ON_NS_xml
  };
  BOOST_STATIC_ASSERT(sizeof(map) / sizeof(map[0]) == 3);
  static const boost::iterator_range<xml_attribute_name_to_id::value_type<SVGPP_ITER_CHAR_TYPE> const *> range 
    = boost::make_iterator_range(map);
  return range;
}

template<>
inline boost::iterator_range<css_property_name_to_id::value_type<SVGPP_ITER_CHAR_TYPE> const *> const & 
  css_property_name_to_id::get_map<SVGPP_ITER_CHAR_TYPE>()
{
  static value_type<SVGPP_ITER_CHAR_TYPE> const map[] = {
#define SVGPP_ON(name, str) 
#define SVGPP_ON_NS(ns, name, str)
#define SVGPP_ON_STYLE(name, str) SVGPP_ADD_ITEM(str, attribute_id_## name)
#include <svgpp/detail/dict/enumerate_all_attributes.inc>
#undef SVGPP_ON
#undef SVGPP_ON_NS
#undef SVGPP_ON_STYLE
  };
  BOOST_STATIC_ASSERT(sizeof(map) / sizeof(map[0]) == styling_attribute_count);
  static const boost::iterator_range<css_property_name_to_id::value_type<SVGPP_ITER_CHAR_TYPE> const *> range 
    = boost::make_iterator_range(map);
  return range;
}

#undef SVGPP_ITER_CHAR_TYPE
#undef SVGPP_ITER_STRINGIZE
#undef SVGPP_ADD_ITEM

#endif // !BOOST_PP_IS_ITERATING