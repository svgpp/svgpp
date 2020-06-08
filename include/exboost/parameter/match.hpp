// Copyright David Abrahams 2005. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef EXBOOST_PARAMETER_MATCH_DWA2005714_HPP_EXBOOST
# define EXBOOST_PARAMETER_MATCH_DWA2005714_HPP_EXBOOST

# include <boost/detail/workaround.hpp>
# include <boost/preprocessor/seq/enum.hpp>

# if BOOST_WORKAROUND(__MWERKS__, <= 0x3003)
// Temporary version of BOOST_PP_SEQ_ENUM until Paul M. integrates the workaround.
#  define EXBOOST_PARAMETER_SEQ_ENUM_I(size,seq) BOOST_PP_CAT(BOOST_PP_SEQ_ENUM_, size) seq
#  define EXBOOST_PARAMETER_SEQ_ENUM(seq) EXBOOST_PARAMETER_SEQ_ENUM_I(BOOST_PP_SEQ_SIZE(seq), seq)
# else
#  define EXBOOST_PARAMETER_SEQ_ENUM(seq) BOOST_PP_SEQ_ENUM(seq)
# endif

# if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))

#  include <exboost/parameter/config.hpp>
#  include <exboost/parameter/aux_/void.hpp>
#  include <boost/preprocessor/arithmetic/sub.hpp>
#  include <boost/preprocessor/facilities/intercept.hpp>
#  include <boost/preprocessor/repetition/enum_trailing_params.hpp>

#  define EXBOOST_PARAMETER_MATCH_DEFAULTS(ArgTypes)              \
        BOOST_PP_ENUM_TRAILING_PARAMS(                          \
            BOOST_PP_SUB(                                       \
                EXBOOST_PARAMETER_MAX_ARITY                       \
              , BOOST_PP_SEQ_SIZE(ArgTypes)                     \
            )                                                   \
          , ::exboost::parameter::void_ BOOST_PP_INTERCEPT   \
        )

# else

#  define EXBOOST_PARAMETER_MATCH_DEFAULTS(ArgTypes)

# endif

//
// Generates, e.g.
//
//    typename dfs_params::match<A1,A2>::type name = dfs_params()
//
// with workarounds for Borland compatibility.
//

# define EXBOOST_PARAMETER_MATCH(ParameterSpec, ArgTypes, name)   \
    typename ParameterSpec ::match<                             \
        EXBOOST_PARAMETER_SEQ_ENUM(ArgTypes)                      \
        EXBOOST_PARAMETER_MATCH_DEFAULTS(ArgTypes)                \
    >::type name = ParameterSpec ()

#endif // EXBOOST_PARAMETER_MATCH_DWA2005714_HPP_EXBOOST
