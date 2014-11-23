// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/parameter.hpp>

namespace svgpp
{

#ifdef BOOST_NO_CXX11_VARIADIC_TEMPLATES

// The simplest way to increase number of svgpp template named parameters is to change BOOST_PARAMETER_MAX_ARITY 
#  ifndef SVGPP_TEMPLATE_PARAMS_NUM
#    define SVGPP_TEMPLATE_PARAMS_NUM BOOST_PARAMETER_MAX_ARITY
#  else
#    if SVGPP_TEMPLATE_PARAMS_NUM > BOOST_PARAMETER_MAX_ARITY
#      error SVGPP_TEMPLATE_PARAMS_NUM must not be set greater than BOOST_PARAMETER_MAX_ARITY
#    endif
#  endif

#  define SVGPP_TEMPLATE_ARGS_DEF \
  BOOST_PP_ENUM_BINARY_PARAMS(SVGPP_TEMPLATE_PARAMS_NUM, class A, = boost::parameter::void_ BOOST_PP_INTERCEPT)

#  define SVGPP_TEMPLATE_ARGS \
  BOOST_PP_ENUM_PARAMS(SVGPP_TEMPLATE_PARAMS_NUM, class A)

#  define SVGPP_TEMPLATE_ARGS_PASS \
  BOOST_PP_ENUM_PARAMS(SVGPP_TEMPLATE_PARAMS_NUM, A)


// For usage in document_traversal which adds one parameter when passes to attribute_dispatcher

#define SVGPP_TEMPLATE_PARAMS_NUM2 BOOST_PP_DEC(SVGPP_TEMPLATE_PARAMS_NUM)

#  define SVGPP_TEMPLATE_ARGS2_DEF \
  BOOST_PP_ENUM_BINARY_PARAMS(SVGPP_TEMPLATE_PARAMS_NUM2, class A, = boost::parameter::void_ BOOST_PP_INTERCEPT)

#  define SVGPP_TEMPLATE_ARGS2 \
  BOOST_PP_ENUM_PARAMS(SVGPP_TEMPLATE_PARAMS_NUM2, class A)

#  define SVGPP_TEMPLATE_ARGS2_PASS \
  BOOST_PP_ENUM_PARAMS(SVGPP_TEMPLATE_PARAMS_NUM2, A)

#else

#  define SVGPP_TEMPLATE_ARGS_DEF  class... Args
#  define SVGPP_TEMPLATE_ARGS      class... Args
#  define SVGPP_TEMPLATE_ARGS_PASS Args...

#  define SVGPP_TEMPLATE_ARGS2_DEF  SVGPP_TEMPLATE_ARGS_DEF
#  define SVGPP_TEMPLATE_ARGS2      SVGPP_TEMPLATE_ARGS
#  define SVGPP_TEMPLATE_ARGS2_PASS SVGPP_TEMPLATE_ARGS_PASS

#endif

BOOST_PARAMETER_TEMPLATE_KEYWORD(angle_factory)
BOOST_PARAMETER_TEMPLATE_KEYWORD(color_factory)
BOOST_PARAMETER_TEMPLATE_KEYWORD(error_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(iri_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(icc_color_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(basic_shapes_events_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(marker_events_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(path_events_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(transform_events_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(value_events_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(viewport_events_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(length_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(markers_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(number_type)
BOOST_PARAMETER_TEMPLATE_KEYWORD(path_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(transform_policy)

}