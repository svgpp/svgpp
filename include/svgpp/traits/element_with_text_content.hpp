#pragma once

#include <svgpp/definitions.hpp>
#include <boost/mpl/bool.hpp>

namespace svgpp { namespace traits 
{

template<class ElementTag>
struct element_with_text_content: boost::mpl::false_ {};

template<> struct element_with_text_content<tag::element::altGlyph  >: boost::mpl::true_ {};
template<> struct element_with_text_content<tag::element::desc      >: boost::mpl::true_ {};
template<> struct element_with_text_content<tag::element::metadata  >: boost::mpl::true_ {};
template<> struct element_with_text_content<tag::element::script    >: boost::mpl::true_ {};
template<> struct element_with_text_content<tag::element::style     >: boost::mpl::true_ {};
template<> struct element_with_text_content<tag::element::text      >: boost::mpl::true_ {};
template<> struct element_with_text_content<tag::element::textPath  >: boost::mpl::true_ {};
template<> struct element_with_text_content<tag::element::title     >: boost::mpl::true_ {};
template<> struct element_with_text_content<tag::element::tspan     >: boost::mpl::true_ {};

}}