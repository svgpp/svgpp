#pragma once

#include <svgpp/detail/literal_values_dictionary.hpp>
#include <svgpp/traits/simple_enumeration_values.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/context_policy_load_value.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/range.hpp>
#include <boost/noncopyable.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace svgpp
{

namespace detail
{

template<class Dictionary, class AttributeTag, class Context, class ValueRange, bool CaseSensitive>
struct simple_enumeration_type_visitor: boost::noncopyable
{
  simple_enumeration_type_visitor(Context & context, ValueRange const & range)
    : context_(context)
    , range_(range)
    , found_(false)
  {}

  template<class T>
  void operator ()(T value_tag) 
  {
    if (!found_ && boost::algorithm::equals(range_, Dictionary::template get_name<T>(), 
      typename boost::mpl::if_c<CaseSensitive, boost::algorithm::is_equal, boost::algorithm::is_iequal>::type()))
    {
      context_policy<tag::load_value_policy, Context>::set(context_, AttributeTag(), value_tag);
      found_ = true;
    }
  }

  bool found() const
  {
    return found_;
  }

private:
  Context & context_;
  ValueRange const & range_;
  bool found_;
};

template<class EnumerationIdMetafunction>
struct simple_enumeration_parser
{
  template<class AttributeTag, class Context, class ValueRange, class PropertySource>
  static bool parse(AttributeTag, Context & context, 
    ValueRange const & attribute_value, PropertySource)
  {
    typedef typename boost::mpl::apply1<EnumerationIdMetafunction, AttributeTag>::type type_id_t;
    typedef literal_values_dictionary<typename boost::range_value<ValueRange>::type> dictionary_t;
    typedef typename traits::simple_enumeration_values<type_id_t>::type tag_list;

    simple_enumeration_type_visitor<
      dictionary_t, AttributeTag, Context, ValueRange,
      boost::is_same<PropertySource, tag::source::attribute>::value
    > fn(
      context, attribute_value);
    boost::mpl::for_each<tag_list>(boost::ref(fn));
    return fn.found();
  }
};

}

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::simple_enumeration, SVGPP_TEMPLATE_ARGS_PASS>
  : detail::simple_enumeration_parser<boost::mpl::quote1<boost::mpl::identity> >
{
  // TODO: error handling
};

template<class ElementTag, SVGPP_TEMPLATE_ARGS>
struct value_parser<boost::mpl::pair<ElementTag, tag::attribute::operator_>, SVGPP_TEMPLATE_ARGS_PASS>
  : detail::simple_enumeration_parser<boost::mpl::bind2<boost::mpl::quote2<boost::mpl::pair>, 
    ElementTag, boost::mpl::placeholders::_1> >
{
};

template<class ElementTag, SVGPP_TEMPLATE_ARGS>
struct value_parser<boost::mpl::pair<ElementTag, tag::attribute::type>, SVGPP_TEMPLATE_ARGS_PASS>
  : detail::simple_enumeration_parser<boost::mpl::bind2<boost::mpl::quote2<boost::mpl::pair>, 
    ElementTag, boost::mpl::placeholders::_1> >
{
};

}