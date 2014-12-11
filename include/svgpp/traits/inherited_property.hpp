// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/traits/attribute_groups.hpp>
#include <boost/mpl/bool.hpp>

namespace svgpp { namespace traits
{
  
template<class AttributeTag>
struct inherited_property: is_presentation_attribute<AttributeTag> {}; 

template<> struct inherited_property<tag::attribute::alignment_baseline>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::baseline_shift>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::clip>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::clip_path>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::display>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::dominant_baseline>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::enable_background>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::filter>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::flood_color>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::flood_opacity>: boost::mpl::false_ {};	
template<> struct inherited_property<tag::attribute::lighting_color>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::mask>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::opacity>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::overflow>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::stop_color>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::stop_opacity>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::text_decoration>: boost::mpl::false_ {};
template<> struct inherited_property<tag::attribute::unicode_bidi>: boost::mpl::false_ {};

}}
