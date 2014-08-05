// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <boost/mpl/empty_sequence.hpp>
#include <boost/mpl/vector.hpp>

namespace svgpp { namespace traits
{

template<class ElementTag> struct element_required_attributes 
{
  typedef boost::mpl::empty_sequence type;
};

template<> struct element_required_attributes<tag::element::rect>
{
  typedef boost::mpl::vector2<tag::attribute::width, tag::attribute::height> type;
};

template<> struct element_required_attributes<tag::element::path>
{
  typedef boost::mpl::vector1<tag::attribute::d> type;
};

template<> struct element_required_attributes<tag::element::circle>
{
  typedef boost::mpl::vector1<tag::attribute::r> type;
};

template<> struct element_required_attributes<tag::element::ellipse>
{
  typedef boost::mpl::vector2<tag::attribute::rx, tag::attribute::ry> type;
};

template<> struct element_required_attributes<tag::element::polyline>
{
  typedef boost::mpl::vector1<tag::attribute::points> type;
};

template<> struct element_required_attributes<tag::element::polygon>
{
  typedef boost::mpl::vector1<tag::attribute::points> type;
};
 
template<> struct element_required_attributes<tag::element::color_profile>
{
  typedef boost::mpl::vector1<tag::attribute::name> type;
};

template<> struct element_required_attributes<tag::element::stop>
{
  typedef boost::mpl::vector1<tag::attribute::offset> type;
};

template<> struct element_required_attributes<tag::element::feBlend>
{
  typedef boost::mpl::vector1<tag::attribute::in2> type;
};

template<> struct element_required_attributes<tag::element::feComposite>
{
  typedef boost::mpl::vector1<tag::attribute::in2> type;
};

template<> struct element_required_attributes<tag::element::feConvolveMatrix>
{
  typedef boost::mpl::vector1<tag::attribute::kernelMatrix> type;
};

template<> struct element_required_attributes<tag::element::feDisplacementMap>
{
  typedef boost::mpl::vector1<tag::attribute::in2> type;
};

template<> struct element_required_attributes<tag::element::feFuncR>
{
  typedef boost::mpl::vector1<tag::attribute::type> type;
};

template<> struct element_required_attributes<tag::element::feFuncG>
{
  typedef boost::mpl::vector1<tag::attribute::type> type;
};

template<> struct element_required_attributes<tag::element::feFuncB>
{
  typedef boost::mpl::vector1<tag::attribute::type> type;
};

template<> struct element_required_attributes<tag::element::feFuncA>
{
  typedef boost::mpl::vector1<tag::attribute::type> type;
};

template<> struct element_required_attributes<tag::element::script>
{
  typedef boost::mpl::vector1<tag::attribute::type> type;
};

template<> struct element_required_attributes<tag::element::font>
{
  typedef boost::mpl::vector1<tag::attribute::horiz_adv_x> type;
};

template<> struct element_required_attributes<tag::element::hkern>
{
  typedef boost::mpl::vector1<tag::attribute::k> type;
};

template<> struct element_required_attributes<tag::element::vkern>
{
  typedef boost::mpl::vector1<tag::attribute::k> type;
};

template<> struct element_required_attributes<tag::element::foreignObject>
{
  typedef boost::mpl::vector2<tag::attribute::width, tag::attribute::height> type;
};

template<> struct element_required_attributes<tag::element::image>
{
  typedef boost::mpl::vector2<tag::attribute::width, tag::attribute::height> type;
};

template<> struct element_required_attributes<tag::element::style>
{
  typedef boost::mpl::vector1<tag::attribute::type> type;
};

}}