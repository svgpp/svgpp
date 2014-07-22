// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/mpl/fold.hpp>
#include <svgpp/definitions.hpp>

namespace svgpp { namespace detail 
{

class check_required_attribute_base
{
public:
  void operator ()(attribute_id) const
  {
  }

  template<class Fn>
  BOOST_CONSTEXPR bool visit_missing(Fn &) const 
  {
    return true;
  }
};

template<class Next, class AttributeTag>
class check_required_attribute: private Next
{
public:
  check_required_attribute()
    : found_(false)
  {
  }

  void operator ()(attribute_id id)
  {
    if (AttributeTag::attribute_id == id)
      found_ = true;
    else
      Next::operator()(id);
  }

  template<class Fn>
  bool visit_missing(Fn & fn) const 
  {
    return Next::visit_missing(fn) && 
      (found_ || fn(AttributeTag()));
  }

private:
  bool found_;
};

template<class RequiredAttributes>
class required_attributes_check:
  public boost::mpl::fold<
    RequiredAttributes, 
    check_required_attribute_base,
    check_required_attribute<boost::mpl::placeholders::_1, boost::mpl::placeholders::_2> >::type
{
};

}}