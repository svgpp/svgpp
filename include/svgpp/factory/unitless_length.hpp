// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/traits/length_units.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/has_key.hpp>
#include <cmath>

namespace svgpp { namespace factory { namespace length
{

template<class LengthType = double, class NumberType = double, class ReferenceAbsoluteUnits = tag::length_units::mm>
class unitless_absolute
{
public:
  typedef LengthType length_type;
  typedef NumberType number_type;

  unitless_absolute()
  {
    // Set 96 PPI as reasonable default
    set_absolute_units_coefficient(96, tag::length_units::in());
  }

  unitless_absolute(number_type reference_to_output_coeff)
    : reference_to_output_coeff_(reference_to_output_coeff)
  {}

  template<class AbsoluteUnits>
  void set_absolute_units_coefficient(number_type coeff, AbsoluteUnits)
  {
    BOOST_MPL_ASSERT((boost::mpl::has_key<traits::absolute_length_units, AbsoluteUnits>));
    typedef typename traits::absolute_length_conversion_coefficient<ReferenceAbsoluteUnits, AbsoluteUnits>::ratio ratio;
    reference_to_output_coeff_ = coeff * ratio::num / ratio::den;
  }

  template<class AbsoluteUnits>
  number_type get_absolute_units_coefficient(AbsoluteUnits) const
  {
    BOOST_MPL_ASSERT((boost::mpl::has_key<traits::absolute_length_units, AbsoluteUnits>));
    typedef typename traits::absolute_length_conversion_coefficient<AbsoluteUnits, ReferenceAbsoluteUnits>::ratio ratio;
    return reference_to_output_coeff_ * ratio::num / ratio::den;
  }

  template<class Units>
  typename boost::enable_if<boost::mpl::has_key<traits::absolute_length_units, Units>, length_type>::type
  create_length(number_type number, Units) const
  {
    typedef typename traits::absolute_length_conversion_coefficient<Units, ReferenceAbsoluteUnits>::ratio ratio;
    return static_cast<length_type>(number * reference_to_output_coeff_ * ratio::num / ratio::den );
  }

private:
  number_type reference_to_output_coeff_;
};

template<class LengthType = double, class NumberType = double>
class unitless_viewport
{
public:
  unitless_viewport(LengthType viewportWidth = 500, LengthType viewportHeight = 500)
  {
    set_viewport_size(viewportWidth, viewportHeight);
  }

  void set_viewport_size(LengthType width, LengthType height)
  {
    // TODO: what if one of the types is integer?
    widthCoefficient_ = width * 0.01;
    heightCoefficient_ = height * 0.01;
    otherCoefficient_ = std::sqrt((width * width + height * height) / 2) * 0.01;
  }

  LengthType create_length(NumberType number, tag::length_units::percent, tag::length_dimension::width) const 
  {
    return static_cast<LengthType>(widthCoefficient_ * number);
  }

  LengthType create_length(NumberType number, tag::length_units::percent, tag::length_dimension::height) const 
  {
    return static_cast<LengthType>(heightCoefficient_ * number);
  }

  LengthType create_length(NumberType number, tag::length_units::percent, tag::length_dimension::not_width_nor_height) const 
  {
    return static_cast<LengthType>(otherCoefficient_ * number);
  }

private:
  NumberType widthCoefficient_, heightCoefficient_, otherCoefficient_;
};

template<class LengthType = double, class NumberType = double>
class unitless_user_units
{
public:
  unitless_user_units(NumberType user_units_coefficient = 1)
    : user_units_coefficient_(user_units_coefficient)
  {}

  LengthType create_length(NumberType number, tag::length_units::px) const
  {
    return number * user_units_coefficient_;
  }

  LengthType create_length(NumberType number, tag::length_units::none) const
  {
    return number * user_units_coefficient_;
  }

  void set_user_units_coefficient(NumberType coeff) { user_units_coefficient_ = coeff; }
  NumberType get_user_units_coefficient() const { return user_units_coefficient_; }

private:
  NumberType user_units_coefficient_;
};

template<class LengthType = double, class NumberType = double, class ReferenceAbsoluteUnits = tag::length_units::mm>
class unitless:
  public unitless_absolute<LengthType, NumberType, ReferenceAbsoluteUnits>,
  public unitless_viewport<LengthType, NumberType>,
  public unitless_user_units<LengthType, NumberType>
{
public:
  using unitless_absolute<LengthType, NumberType, ReferenceAbsoluteUnits>::create_length;
  using unitless_viewport<LengthType, NumberType>::create_length;
  using unitless_user_units<LengthType, NumberType>::create_length;

  unitless()
    : em_coefficient_(10)
    , ex_coefficient_(10)
  {}

  LengthType create_length(NumberType number, tag::length_units::em) const
  {
    return number * em_coefficient_;
  }

  LengthType create_length(NumberType number, tag::length_units::ex) const
  {
    return number * ex_coefficient_;
  }

  void set_em_coefficient(NumberType coeff)
  { 
    em_coefficient_ = coeff; 
  }

  void set_ex_coefficient(NumberType coeff)
  { 
    ex_coefficient_ = coeff; 
  }

  void set_em_coefficient(NumberType coeff, tag::length_units::px)
  {
    em_coefficient_ = this->get_user_units_coefficient() * coeff;
  }

  void set_ex_coefficient(NumberType coeff, tag::length_units::px)
  {
    ex_coefficient_ = this->get_user_units_coefficient() * coeff;
  }

  template<class AbsoluteUnits>
  typename boost::enable_if<boost::mpl::has_key<traits::absolute_length_units, AbsoluteUnits> >::type
  set_em_coefficient(NumberType coeff, AbsoluteUnits unitsTag)
  {
    em_coefficient_ = coeff * this->get_absolute_units_coefficient(unitsTag);
  }

  template<class AbsoluteUnits>
  typename boost::enable_if<boost::mpl::has_key<traits::absolute_length_units, AbsoluteUnits> >::type
  set_ex_coefficient(NumberType coeff, AbsoluteUnits unitsTag)
  {
    ex_coefficient_ = coeff * this->get_absolute_units_coefficient(unitsTag);
  }

  // Optional length_to_user_coordinate interface that may be used when converting
  // basic shapes coordinates to path in some adapters
  typedef LengthType coordinate_type;
  static coordinate_type length_to_user_coordinate(LengthType length, tag::length_dimension::any const &) 
  {
    return length;
  }

private:
  NumberType em_coefficient_, ex_coefficient_;
};

typedef unitless<> default_factory;

}}}