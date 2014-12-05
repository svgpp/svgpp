// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/parameter.hpp>
#include <boost/noncopyable.hpp>
#include <svgpp/policy/detail/css_name_to_id.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(xml_attribute_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(css_name_to_id_policy)

namespace detail
{
  template<class ErrorPolicy>
  class missing_attribute_visitor: boost::noncopyable
  {
  public:
    missing_attribute_visitor(typename ErrorPolicy::context_type & context)
      : context_(context)
    {
    }

    template<class AttributeTag>
    bool operator()(AttributeTag attribute) const
    {
      return ErrorPolicy::required_attribute_not_found(context_, attribute);
    }

  private:
    typename ErrorPolicy::context_type & context_;
  };
}

}