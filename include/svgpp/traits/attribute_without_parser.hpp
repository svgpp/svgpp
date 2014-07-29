// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/traits/attribute_groups.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/utility/enable_if.hpp>

namespace svgpp { namespace traits
{

// TODO: replace attribute_without_parser with something more meaningful

template<class AttributeTag, class Enable = void>
struct attribute_without_parser: boost::mpl::false_ {};

template<class AttributeTag>
struct attribute_without_parser<AttributeTag,
  typename boost::enable_if<boost::mpl::has_key<traits::graphical_event_attributes, AttributeTag> >::type >
  : boost::mpl::true_ {};

template<class AttributeTag>
struct attribute_without_parser<AttributeTag,
  typename boost::enable_if<boost::mpl::has_key<traits::document_event_attributes, AttributeTag> >::type >
  : boost::mpl::true_ {};

template<> struct attribute_without_parser<tag::attribute::baseProfile         > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::contentScriptType   > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::contentStyleType    > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::id                  > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::attributeName       > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::font_family         > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::format              > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::glyphRef            > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::lang                > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::local               > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::media               > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::requiredExtensions  > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::requiredFeatures    > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::result              > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::string              > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::style               > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::systemLanguage      > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::target              > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::title               > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::unicode             > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::viewTarget          > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::begin               > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::end                 > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::by                  > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::from                > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::to                  > : boost::mpl::true_ {};
template<> struct attribute_without_parser<tag::attribute::xlink::title        > : boost::mpl::true_ {};

}}