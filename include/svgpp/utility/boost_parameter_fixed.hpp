// Copyright Oleg Maximenko 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/version.hpp>

#if BOOST_VERSION < 107100

#include <boost/parameter.hpp>

namespace exboost {
namespace parameter = ::boost::parameter;
}  // namespace exboost

#define EXBOOST_PARAMETER_TEMPLATE_KEYWORD(arg) \
  BOOST_PARAMETER_TEMPLATE_KEYWORD(arg)

#else  // !BOOST_VERSION >= 107100

#ifdef BOOST_PARAMETER_MAX_ARITY
#define EXBOOST_PARAMETER_MAX_ARITY BOOST_PARAMETER_MAX_ARITY
#endif

#include <exboost/parameter.hpp>

#endif  // !BOOST_VERSION >= 107100
