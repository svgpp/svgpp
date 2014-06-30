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

#else

#  define SVGPP_TEMPLATE_ARGS_DEF class... Args
#  define SVGPP_TEMPLATE_ARGS     class... Args
#  define SVGPP_TEMPLATE_ARGS_PASS Args...

#endif

BOOST_PARAMETER_TEMPLATE_KEYWORD(number_type)
BOOST_PARAMETER_TEMPLATE_KEYWORD(error_policy)

namespace detail
{
  struct parameter_not_set_tag;
}

}