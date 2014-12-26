// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/detail/literal_values_dictionary.hpp>
#include <svgpp/traits/literal_enumeration_values.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
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

template<
  class Dictionary, 
  class AttributeTag, 
  class Context, 
  class ValueEventsPolicy, 
  class ValueRange, 
  class PropertySource
>
struct literal_enumeration_type_visitor: boost::noncopyable
{
  literal_enumeration_type_visitor(Context & context, ValueRange const & range)
    : context_(context)
    , range_(range)
    , found_(false)
  {}

  template<class T>
  void operator ()(T value_tag) 
  {
    if (!found_ && boost::algorithm::equals(range_, Dictionary::template get_name<T>(), 
      typename boost::mpl::if_<
        boost::is_same<PropertySource, tag::source::attribute>, 
        boost::algorithm::is_equal, 
        boost::algorithm::is_iequal
      >::type()))
    {
      ValueEventsPolicy::set(context_, AttributeTag(), PropertySource(), value_tag);
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

}

template<class LiteralsList, SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::literal_enumeration<LiteralsList>, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class ValueRange, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, ValueRange const & attribute_value, PropertySource)
  {
    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef detail::literal_values_dictionary<typename boost::range_value<ValueRange>::type> dictionary_t;

    detail::literal_enumeration_type_visitor<
      dictionary_t, 
      AttributeTag, 
      typename args_t::value_events_context::type, 
      typename args_t::value_events_policy,
      ValueRange,
      PropertySource
    > fn(args_t::value_events_context::get(context), attribute_value);

    boost::mpl::for_each<LiteralsList>(boost::ref(fn));
    if (fn.found())
      return true;
    else
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
  }
};

}