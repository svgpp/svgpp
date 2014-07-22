// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/set.hpp>
#include <boost/ratio/ratio.hpp>

namespace svgpp { namespace traits
{

typedef boost::mpl::set5<
  tag::length_units::in,
  tag::length_units::cm,
  tag::length_units::mm,
  tag::length_units::pt,
  tag::length_units::pc
> absolute_length_units;

typedef boost::mpl::joint_view<
  absolute_length_units,
  boost::mpl::set5<
    tag::length_units::em,
    tag::length_units::ex,
    tag::length_units::px,
    tag::length_units::percent,
    tag::length_units::none
> > all_length_units;

template<class Src, class Dst>
struct absolute_length_conversion_coefficient
{ 
  typedef typename boost::ratio_divide<
    typename absolute_length_conversion_coefficient<tag::length_units::in, Dst>::ratio,
    typename absolute_length_conversion_coefficient<tag::length_units::in, Src>::ratio
  > ratio; 
};

template<class Src>
struct absolute_length_conversion_coefficient<Src, Src>
{ typedef boost::ratio<1> ratio; };

template<>
struct absolute_length_conversion_coefficient<tag::length_units::in, tag::length_units::cm>
{ typedef boost::ratio<254, 100> ratio; };

template<>
struct absolute_length_conversion_coefficient<tag::length_units::in, tag::length_units::mm>
{ typedef boost::ratio<254, 10> ratio; };

template<>
struct absolute_length_conversion_coefficient<tag::length_units::in, tag::length_units::pt>
{ typedef boost::ratio<72> ratio; };

template<>
struct absolute_length_conversion_coefficient<tag::length_units::in, tag::length_units::pc>
{ typedef boost::ratio<6> ratio; };

}}