// Copyright Daniel Wallin 2005. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EXBOOST_PARAMETER_FLATTEN_051217_HPP_EXBOOST
# define EXBOOST_PARAMETER_FLATTEN_051217_HPP_EXBOOST

# include <boost/preprocessor/tuple/elem.hpp>
# include <boost/preprocessor/tuple/rem.hpp>
# include <boost/preprocessor/cat.hpp>
# include <boost/preprocessor/seq/for_each.hpp>
# include <boost/preprocessor/seq/for_each_i.hpp>
# include <boost/preprocessor/identity.hpp>
# include <boost/preprocessor/selection/max.hpp>
# include <boost/preprocessor/arithmetic/sub.hpp>
# include <boost/preprocessor/repetition/enum_trailing.hpp>
# include <exboost/parameter/aux_/preprocessor/for_each.hpp>

# define EXBOOST_PARAMETER_FLATTEN_SPLIT_required required,
# define EXBOOST_PARAMETER_FLATTEN_SPLIT_optional optional,
# define EXBOOST_PARAMETER_FLATTEN_SPLIT_deduced deduced,

# define EXBOOST_PARAMETER_FLATTEN_SPLIT(sub) \
    BOOST_PP_CAT(EXBOOST_PARAMETER_FLATTEN_SPLIT_, sub)

# define EXBOOST_PARAMETER_FLATTEN_QUALIFIER(sub) \
    BOOST_PP_SPLIT(0, EXBOOST_PARAMETER_FLATTEN_SPLIT(sub))

# define EXBOOST_PARAMETER_FLATTEN_ARGS(sub) \
    BOOST_PP_SPLIT(1, EXBOOST_PARAMETER_FLATTEN_SPLIT(sub))

# define EXBOOST_PARAMETER_FLATTEN_ARITY_optional(arities) \
    BOOST_PP_TUPLE_ELEM(3,0,arities)

# define EXBOOST_PARAMETER_FLATTEN_ARITY_required(arities) \
    BOOST_PP_TUPLE_ELEM(3,1,arities)

# define EXBOOST_PARAMETER_FLATTEN_SPEC0_DUMMY_ELEM(z, n, data) ~
# define EXBOOST_PARAMETER_FLATTEN_SPEC0(r, n, elem, data) \
    (( \
        BOOST_PP_TUPLE_ELEM(3,2,data) \
      , BOOST_PP_TUPLE_REM(BOOST_PP_TUPLE_ELEM(3,0,data)) elem \
        BOOST_PP_ENUM_TRAILING( \
            BOOST_PP_SUB( \
                BOOST_PP_TUPLE_ELEM(3,1,data) \
              , BOOST_PP_TUPLE_ELEM(3,0,data) \
            ) \
          , EXBOOST_PARAMETER_FLATTEN_SPEC0_DUMMY_ELEM \
          , ~ \
        ) \
    ))

# define EXBOOST_PARAMETER_FLATTEN_SPEC_AUX(r, arity, max_arity, spec, transform) \
    EXBOOST_PARAMETER_FOR_EACH_R( \
        r \
      , arity \
      , EXBOOST_PARAMETER_FLATTEN_ARGS(spec) \
      , (arity, max_arity, transform(EXBOOST_PARAMETER_FLATTEN_QUALIFIER(spec))) \
      , EXBOOST_PARAMETER_FLATTEN_SPEC0 \
    )

# define EXBOOST_PARAMETER_FLATTEN_IDENTITY(x) x

# define EXBOOST_PARAMETER_FLATTEN_SPEC_optional(r, arities, spec) \
    EXBOOST_PARAMETER_FLATTEN_SPEC_AUX( \
        r \
      , BOOST_PP_CAT( \
            EXBOOST_PARAMETER_FLATTEN_ARITY_, EXBOOST_PARAMETER_FLATTEN_QUALIFIER(spec) \
        )(arities) \
      , BOOST_PP_TUPLE_ELEM(3,2,arities) \
      , spec \
      , EXBOOST_PARAMETER_FLATTEN_IDENTITY \
    )

# define EXBOOST_PARAMETER_FLATTEN_SPEC_required(r, arities, spec) \
    EXBOOST_PARAMETER_FLATTEN_SPEC_optional(r, arities, spec)

# define EXBOOST_PARAMETER_FLATTEN_SPEC_AS_DEDUCED(x) BOOST_PP_CAT(deduced_,x)

# define EXBOOST_PARAMETER_FLATTEN_SPEC_deduced_M(r, arities, n, spec) \
    EXBOOST_PARAMETER_FLATTEN_SPEC_AUX( \
        r \
      , BOOST_PP_CAT( \
            EXBOOST_PARAMETER_FLATTEN_ARITY_, EXBOOST_PARAMETER_FLATTEN_QUALIFIER(spec) \
        )(arities) \
      , BOOST_PP_TUPLE_ELEM(3,2,arities) \
      , spec \
      , EXBOOST_PARAMETER_FLATTEN_SPEC_AS_DEDUCED \
    )

# define EXBOOST_PARAMETER_FLATTEN_SPEC_deduced(r, arities, spec) \
    BOOST_PP_SEQ_FOR_EACH_I_R( \
        r \
      , EXBOOST_PARAMETER_FLATTEN_SPEC_deduced_M \
      , arities \
      , EXBOOST_PARAMETER_FLATTEN_ARGS(spec) \
    )

# define EXBOOST_PARAMETER_FLATTEN_SPEC(r, arities, spec) \
    BOOST_PP_CAT( \
        EXBOOST_PARAMETER_FLATTEN_SPEC_, EXBOOST_PARAMETER_FLATTEN_QUALIFIER(spec) \
    )(r, arities, spec)

# define EXBOOST_PARAMETER_FLATTEN(optional_arity, required_arity, wanted_arity, specs) \
    BOOST_PP_SEQ_FOR_EACH( \
        EXBOOST_PARAMETER_FLATTEN_SPEC \
      , ( \
            optional_arity, required_arity \
          , wanted_arity \
        ) \
      , specs \
    )

#endif // EXBOOST_PARAMETER_FLATTEN_051217_HPP_EXBOOST

