// Copyright Daniel Wallin 2006. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EXBOOST_PARAMETER_PREPROCESSOR_060206_HPP_EXBOOST
# define EXBOOST_PARAMETER_PREPROCESSOR_060206_HPP_EXBOOST

# include <exboost/parameter/parameters.hpp>
# include <exboost/parameter/binding.hpp>
# include <exboost/parameter/match.hpp>

# include <exboost/parameter/aux_/parenthesized_type.hpp>
# include <exboost/parameter/aux_/cast.hpp>
# include <exboost/parameter/aux_/preprocessor/flatten.hpp>

# include <boost/preprocessor/repetition/repeat_from_to.hpp>
# include <boost/preprocessor/comparison/equal.hpp>
# include <boost/preprocessor/control/if.hpp>
# include <boost/preprocessor/control/iif.hpp>
# include <boost/preprocessor/control/expr_if.hpp>
# include <boost/preprocessor/repetition/enum_params.hpp>
# include <boost/preprocessor/repetition/enum_binary_params.hpp>
# include <boost/preprocessor/repetition/enum_trailing.hpp>
# include <boost/preprocessor/seq/first_n.hpp>
# include <boost/preprocessor/seq/for_each_product.hpp>
# include <boost/preprocessor/seq/for_each_i.hpp>
# include <boost/preprocessor/tuple/elem.hpp>
# include <boost/preprocessor/tuple/eat.hpp>
# include <boost/preprocessor/seq/fold_left.hpp>
# include <boost/preprocessor/seq/push_back.hpp>
# include <boost/preprocessor/seq/size.hpp>
# include <boost/preprocessor/seq/enum.hpp>
# include <boost/preprocessor/seq/push_back.hpp>

# include <boost/preprocessor/detail/is_nullary.hpp>

# include <boost/mpl/always.hpp>
# include <boost/mpl/apply_wrap.hpp>

namespace exboost { using namespace boost; namespace parameter { namespace aux {

#  if ! defined(BOOST_NO_SFINAE) && ! BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x592))

// Given Match, which is "void x" where x is an argument matching
// criterion, extract a corresponding MPL predicate.
template <class Match>
struct unwrap_predicate;

// Match anything
template <>
struct unwrap_predicate<void*>
{
    typedef mpl::always<mpl::true_> type;
};

#if BOOST_WORKAROUND(__SUNPRO_CC, BOOST_TESTED_AT(0x580))

typedef void* voidstar;

// A matching predicate is explicitly specified
template <class Predicate>
struct unwrap_predicate<voidstar (Predicate)>
{
    typedef Predicate type;
};

#else

// A matching predicate is explicitly specified
template <class Predicate>
struct unwrap_predicate<void *(Predicate)>
{
    typedef Predicate type;
};

#endif


// A type to which the argument is supposed to be convertible is
// specified
template <class Target>
struct unwrap_predicate<void (Target)>
{
    typedef is_convertible<mpl::_, Target> type;
};

// Recast the ParameterSpec's nested match metafunction as a free metafunction
template <
    class Parameters
  , BOOST_PP_ENUM_BINARY_PARAMS(
        EXBOOST_PARAMETER_MAX_ARITY, class A, = exboost::parameter::void_ BOOST_PP_INTERCEPT
    )
>
struct match
  : Parameters::template match<
        BOOST_PP_ENUM_PARAMS(EXBOOST_PARAMETER_MAX_ARITY, A)
    >
{};
# endif

# undef false_

template <
    class Parameters
  , BOOST_PP_ENUM_BINARY_PARAMS(
        EXBOOST_PARAMETER_MAX_ARITY, class A, = exboost::parameter::void_ BOOST_PP_INTERCEPT
    )
>
struct argument_pack
{
    typedef typename make_arg_list<
        typename EXBOOST_PARAMETER_build_arg_list(
            EXBOOST_PARAMETER_MAX_ARITY, make_items, typename Parameters::parameter_spec, A
        )::type
      , typename Parameters::deduced_list
      , tag_keyword_arg
      , mpl::false_
    >::type result;
    typedef typename mpl::first<result>::type type;
};

// Works around VC6 problem where it won't accept rvalues.
template <class T>
T& as_lvalue(T& value, long)
{
    return value;
}

template <class T>
T const& as_lvalue(T const& value, int)
{
    return value;
}


# if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))

template <class Predicate, class T, class Args>
struct apply_predicate
{
    BOOST_MPL_ASSERT((
        mpl::and_<mpl::false_,T>
    ));

    typedef typename mpl::if_<
        typename mpl::apply2<Predicate,T,Args>::type
      , char
      , int
    >::type type;
};

template <class P>
struct funptr_predicate
{
    static P p;

    template <class T, class Args, class P0>
    static typename apply_predicate<P0,T,Args>::type
    check_predicate(type<T>, Args*, void**(*)(P0));

    template <class T, class Args, class P0>
    static typename mpl::if_<
        is_convertible<T,P0>
      , char
      , int
     >::type check_predicate(type<T>, Args*, void*(*)(P0));

    template <class T, class Args>
    struct apply
    {
        BOOST_STATIC_CONSTANT(bool, result =
            sizeof(check_predicate(boost::type<T>(), (Args*)0, &p)) == 1
        );

        typedef mpl::bool_<apply<T,Args>::result> type;
    };
};

template <>
struct funptr_predicate<void**>
  : mpl::always<mpl::true_>
{};

# endif

}}} // namespace exboost::parameter::aux

# if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))
// From Paul Mensonides
#  define EXBOOST_PARAMETER_IS_NULLARY(x) \
    BOOST_PP_SPLIT(1, EXBOOST_PARAMETER_IS_NULLARY_C x BOOST_PP_COMMA() 0) \
    /**/
#  define EXBOOST_PARAMETER_IS_NULLARY_C() \
    ~, 1 BOOST_PP_RPAREN() \
    BOOST_PP_TUPLE_EAT(2) BOOST_PP_LPAREN() ~ \
    /**/
# else
#  define EXBOOST_PARAMETER_IS_NULLARY(x) BOOST_PP_IS_NULLARY(x)
# endif

# define EXBOOST_PARAMETER_MEMBER_FUNCTION_CHECK_STATIC_static ()
# define EXBOOST_PARAMETER_MEMBER_FUNCTION_IS_STATIC(name) \
    EXBOOST_PARAMETER_IS_NULLARY( \
        BOOST_PP_CAT(EXBOOST_PARAMETER_MEMBER_FUNCTION_CHECK_STATIC_,name) \
    )

# if !defined(BOOST_MSVC)
#  define EXBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC_static
#  define EXBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC(name) \
    BOOST_PP_CAT(EXBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC_, name)
# else
// Workaround for MSVC preprocessor.
//
// When stripping static from "static f", msvc will produce
// " f". The leading whitespace doesn't go away when pasting
// the token with something else, so this thing is a hack to
// strip the whitespace.
#  define EXBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC_static (
#  define EXBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC_AUX(name) \
    BOOST_PP_CAT(EXBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC_, name))
#  define EXBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC(name) \
    BOOST_PP_SEQ_HEAD( \
        EXBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC_AUX(name) \
    )
# endif

# define EXBOOST_PARAMETER_MEMBER_FUNCTION_STATIC(name) \
    BOOST_PP_EXPR_IF( \
        EXBOOST_PARAMETER_MEMBER_FUNCTION_IS_STATIC(name) \
      , static \
    )

# define EXBOOST_PARAMETER_MEMBER_FUNCTION_NAME(name) \
    BOOST_PP_IF( \
        EXBOOST_PARAMETER_MEMBER_FUNCTION_IS_STATIC(name) \
      , EXBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC \
      , name BOOST_PP_TUPLE_EAT(1) \
    )(name)

// Calculates [begin, end) arity range.

# define EXBOOST_PARAMETER_ARITY_RANGE_M_optional(state) state
# define EXBOOST_PARAMETER_ARITY_RANGE_M_deduced_optional(state) state
# define EXBOOST_PARAMETER_ARITY_RANGE_M_required(state) BOOST_PP_INC(state)
# define EXBOOST_PARAMETER_ARITY_RANGE_M_deduced_required(state) BOOST_PP_INC(state)

# define EXBOOST_PARAMETER_ARITY_RANGE_M(s, state, x) \
    BOOST_PP_CAT( \
        EXBOOST_PARAMETER_ARITY_RANGE_M_ \
      , EXBOOST_PARAMETER_FN_ARG_QUALIFIER(x) \
    )(state)
/**/

# define EXBOOST_PARAMETER_ARITY_RANGE(args) \
    ( \
        BOOST_PP_SEQ_FOLD_LEFT(EXBOOST_PARAMETER_ARITY_RANGE_M, 0, args) \
      , BOOST_PP_INC(BOOST_PP_SEQ_SIZE(args)) \
    )
/**/

// Accessor macros for the argument specs tuple.
# define EXBOOST_PARAMETER_FN_ARG_QUALIFIER(x) \
    BOOST_PP_TUPLE_ELEM(4,0,x)
/**/

# define EXBOOST_PARAMETER_FN_ARG_NAME(x) \
    BOOST_PP_TUPLE_ELEM(4,1,x)
/**/

# define EXBOOST_PARAMETER_FN_ARG_PRED(x) \
    BOOST_PP_TUPLE_ELEM(4,2,x)
/**/

# define EXBOOST_PARAMETER_FN_ARG_DEFAULT(x) \
    BOOST_PP_TUPLE_ELEM(4,3,x)
/**/

# define BOOST_PARAMETETER_FUNCTION_EAT_KEYWORD_QUALIFIER_out(x)
# define BOOST_PARAMETETER_FUNCTION_EAT_KEYWORD_QUALIFIER_in_out(x)

// Returns 1 if x is either "out(k)" or "in_out(k)".
# define EXBOOST_PARAMETER_FUNCTION_IS_KEYWORD_QUALIFIER(x) \
    BOOST_PP_IS_EMPTY( \
        BOOST_PP_CAT(BOOST_PARAMETETER_FUNCTION_EAT_KEYWORD_QUALIFIER_, x) \
    ) \
/**/

# define BOOST_PARAMETETER_FUNCTION_GET_KEYWORD_QUALIFIER_out(x) x
# define BOOST_PARAMETETER_FUNCTION_GET_KEYWORD_QUALIFIER_in_out(x) x
# define EXBOOST_PARAMETER_FUNCTION_KEYWORD_GET(x) \
    BOOST_PP_CAT(BOOST_PARAMETETER_FUNCTION_GET_KEYWORD_QUALIFIER_, x)
/**/

// Returns the keyword of x, where x is either a keyword qualifier
// or a keyword.
//
//   k => k
//   out(k) => k
//   in_out(k) => k
//
# define EXBOOST_PARAMETER_FUNCTION_KEYWORD(x) \
    BOOST_PP_IF( \
        EXBOOST_PARAMETER_FUNCTION_IS_KEYWORD_QUALIFIER(x) \
      , EXBOOST_PARAMETER_FUNCTION_KEYWORD_GET \
      , x BOOST_PP_TUPLE_EAT(1) \
    )(x)
/**/

# define EXBOOST_PARAMETER_FN_ARG_KEYWORD(x) \
    EXBOOST_PARAMETER_FUNCTION_KEYWORD( \
        EXBOOST_PARAMETER_FN_ARG_NAME(x) \
    )

// Builds forwarding functions.

# define EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_TEMPLATE_Z(z, n) \
    template<BOOST_PP_ENUM_PARAMS_Z(z, n, class ParameterArgumentType)>
/**/

# if ! defined(BOOST_NO_SFINAE) && ! BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x592))
#  define EXBOOST_PARAMETER_FUNCTION_FWD_MATCH_Z(z, name, parameters, n) \
    , typename exboost::parameter::aux::match< \
          parameters, BOOST_PP_ENUM_PARAMS(n, ParameterArgumentType) \
      >::type = parameters()
# else
#  define EXBOOST_PARAMETER_FUNCTION_FWD_MATCH_Z(z, name, parameters, n)
# endif
/**/

# define EXBOOST_PARAMETER_FUNCTION_PARAMETERS_NAME(base) \
    BOOST_PP_CAT( \
        boost_param_parameters_ \
      , BOOST_PP_CAT(__LINE__, EXBOOST_PARAMETER_MEMBER_FUNCTION_NAME(base)) \
    )

// Produce a name for a result type metafunction for the function
// named base
# define EXBOOST_PARAMETER_FUNCTION_RESULT_NAME(base) \
    BOOST_PP_CAT( \
        boost_param_result_ \
      , BOOST_PP_CAT(__LINE__,EXBOOST_PARAMETER_MEMBER_FUNCTION_NAME(base)) \
    )

// Can't do boost_param_impl_ ## base because base might start with an underscore
// daniel: what? how is that relevant? the reason for using CAT() is to make sure
// base is expanded. i'm not sure we need to here, but it's more stable to do it.
# define EXBOOST_PARAMETER_IMPL(base) \
    BOOST_PP_CAT(boost_param_impl,EXBOOST_PARAMETER_MEMBER_FUNCTION_NAME(base))

# define EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION00(z, n, r, data, elem) \
    BOOST_PP_IF( \
        n \
      , EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_TEMPLATE_Z, BOOST_PP_TUPLE_EAT(2) \
    )(z,n) \
    EXBOOST_PARAMETER_MEMBER_FUNCTION_STATIC(BOOST_PP_TUPLE_ELEM(7,3,data)) \
    inline \
    BOOST_PP_EXPR_IF(n, typename) \
        EXBOOST_PARAMETER_FUNCTION_RESULT_NAME(BOOST_PP_TUPLE_ELEM(7,3,data))<   \
        BOOST_PP_EXPR_IF(n, typename) \
        exboost::parameter::aux::argument_pack< \
            EXBOOST_PARAMETER_FUNCTION_PARAMETERS_NAME(BOOST_PP_TUPLE_ELEM(7,3,data)) \
            BOOST_PP_COMMA_IF(n) \
            BOOST_PP_IF( \
                n, BOOST_PP_SEQ_ENUM, BOOST_PP_TUPLE_EAT(1) \
            )(elem) \
        >::type \
    >::type \
    EXBOOST_PARAMETER_MEMBER_FUNCTION_NAME(BOOST_PP_TUPLE_ELEM(7,3,data))( \
        BOOST_PP_IF( \
            n \
          , BOOST_PP_SEQ_FOR_EACH_I_R \
          , BOOST_PP_TUPLE_EAT(4) \
        )( \
            r \
          , EXBOOST_PARAMETER_FUNCTION_ARGUMENT \
          , ~ \
          , elem \
        ) \
        BOOST_PP_IF(n, EXBOOST_PARAMETER_FUNCTION_FWD_MATCH_Z, BOOST_PP_TUPLE_EAT(4))( \
            z \
          , BOOST_PP_TUPLE_ELEM(7,3,data) \
          , EXBOOST_PARAMETER_FUNCTION_PARAMETERS_NAME(BOOST_PP_TUPLE_ELEM(7,3,data)) \
          , n \
        ) \
    ) BOOST_PP_EXPR_IF(BOOST_PP_TUPLE_ELEM(7,4,data), const) \
    { \
        return EXBOOST_PARAMETER_IMPL(BOOST_PP_TUPLE_ELEM(7,3,data))( \
            EXBOOST_PARAMETER_FUNCTION_PARAMETERS_NAME(BOOST_PP_TUPLE_ELEM(7,3,data))()( \
                BOOST_PP_ENUM_PARAMS_Z(z, n, a) \
            ) \
        ); \
    }
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION0(r, data, elem) \
    EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION00( \
        BOOST_PP_TUPLE_ELEM(7,0,data) \
      , BOOST_PP_TUPLE_ELEM(7,1,data) \
      , r \
      , data \
      , elem \
    )
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_ARITY_0(z, n, data) \
    EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION00( \
        z, n, BOOST_PP_DEDUCE_R() \
      , (z, n, BOOST_PP_TUPLE_REM(5) data) \
      , ~ \
    )
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_ARITY_N(z, n, data) \
    BOOST_PP_SEQ_FOR_EACH( \
        EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION0 \
      , (z, n, BOOST_PP_TUPLE_REM(5) data) \
      , BOOST_PP_SEQ_FOR_EACH_PRODUCT( \
            EXBOOST_PARAMETER_FUNCTION_FWD_PRODUCT \
          , BOOST_PP_SEQ_FIRST_N( \
                n, BOOST_PP_TUPLE_ELEM(5,3,data) \
            ) \
        ) \
    )
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION(z, n, data) \
    BOOST_PP_IF( \
        n \
      , EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_ARITY_N \
      , EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_ARITY_0 \
    )(z,n,data) \
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS0( \
    result,name,args,const_,combinations,range \
) \
    BOOST_PP_REPEAT_FROM_TO( \
        BOOST_PP_TUPLE_ELEM(2,0,range), BOOST_PP_TUPLE_ELEM(2,1,range) \
      , EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION \
      , (result,name,const_,combinations,BOOST_PP_TUPLE_ELEM(2,1,range)) \
    )
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS(result,name,args, const_, combinations) \
    EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS0( \
        result, name, args, const_, combinations, EXBOOST_PARAMETER_ARITY_RANGE(args) \
    )
/**/

// Builds exboost::parameter::parameters<> specialization
#  define EXBOOST_PARAMETER_FUNCTION_PARAMETERS_QUALIFIER_optional(tag) \
    optional<tag

#  define EXBOOST_PARAMETER_FUNCTION_PARAMETERS_QUALIFIER_required(tag) \
    required<tag

#  define EXBOOST_PARAMETER_FUNCTION_PARAMETERS_QUALIFIER_deduced_optional(tag) \
    optional<exboost::parameter::deduced<tag>

#  define EXBOOST_PARAMETER_FUNCTION_PARAMETERS_QUALIFIER_deduced_required(tag) \
    required<exboost::parameter::deduced<tag>

# if !BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))

#  define EXBOOST_PARAMETER_FUNCTION_PARAMETERS_M(r,tag_namespace,i,elem) \
    BOOST_PP_COMMA_IF(i) \
    exboost::parameter::BOOST_PP_CAT( \
        EXBOOST_PARAMETER_FUNCTION_PARAMETERS_QUALIFIER_ \
      , EXBOOST_PARAMETER_FN_ARG_QUALIFIER(elem) \
    )( \
        tag_namespace::EXBOOST_PARAMETER_FUNCTION_KEYWORD( \
            EXBOOST_PARAMETER_FN_ARG_KEYWORD(elem) \
        ) \
    ) \
      , typename exboost::parameter::aux::unwrap_predicate< \
            void EXBOOST_PARAMETER_FN_ARG_PRED(elem) \
        >::type \
    >
# elif BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))
#  define EXBOOST_PARAMETER_FUNCTION_PARAMETERS_M(r,tag_namespace,i,elem) \
    BOOST_PP_COMMA_IF(i) \
    exboost::parameter::BOOST_PP_CAT( \
        EXBOOST_PARAMETER_FUNCTION_PARAMETERS_QUALIFIER_ \
      , EXBOOST_PARAMETER_FN_ARG_QUALIFIER(elem) \
    )( \
        tag_namespace::EXBOOST_PARAMETER_FUNCTION_KEYWORD( \
            EXBOOST_PARAMETER_FN_ARG_KEYWORD(elem) \
        ) \
    ) \
      , boost::mpl::always<boost::mpl::true_> \
    >
# endif

# define EXBOOST_PARAMETER_FUNCTION_PARAMETERS(tag_namespace, base, args)             \
    template <class BoostParameterDummy>                                            \
    struct BOOST_PP_CAT(                                                            \
            BOOST_PP_CAT(boost_param_params_, __LINE__)                             \
          , EXBOOST_PARAMETER_MEMBER_FUNCTION_NAME(base)                              \
    ) : exboost::parameter::parameters<                                               \
            BOOST_PP_SEQ_FOR_EACH_I(                                                \
                EXBOOST_PARAMETER_FUNCTION_PARAMETERS_M, tag_namespace, args          \
            )                                                                       \
        >                                                                           \
    {};                                                                             \
                                                                                    \
    typedef BOOST_PP_CAT( \
            BOOST_PP_CAT(boost_param_params_, __LINE__) \
          , EXBOOST_PARAMETER_MEMBER_FUNCTION_NAME(base) \
    )<int>

// Defines result type metafunction
# define EXBOOST_PARAMETER_FUNCTION_RESULT_ARG(z, _, i, x) \
    BOOST_PP_COMMA_IF(i) class BOOST_PP_TUPLE_ELEM(3,1,x)
/**/

# define EXBOOST_PARAMETER_FUNCTION_RESULT_(result, name, args)                                   \
    template <class Args>                                                                       \
    struct EXBOOST_PARAMETER_FUNCTION_RESULT_NAME(name)                                           \
    {                                                                                           \
        typedef typename EXBOOST_PARAMETER_PARENTHESIZED_TYPE(result) type;                       \
    };

// Defines implementation function
# define EXBOOST_PARAMETER_FUNCTION_IMPL_HEAD(name)           \
    template <class Args>                                   \
    typename EXBOOST_PARAMETER_FUNCTION_RESULT_NAME(name)<    \
       Args                                                 \
    >::type EXBOOST_PARAMETER_IMPL(name)(Args const& args)

# define EXBOOST_PARAMETER_FUNCTION_IMPL_FWD(name) \
    EXBOOST_PARAMETER_FUNCTION_IMPL_HEAD(name);
/**/

# define EXBOOST_PARAMETER_FUNCTION_SPLIT_ARG_required(state, arg) \
    ( \
        BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(4, 0, state)) \
      , BOOST_PP_SEQ_PUSH_BACK(BOOST_PP_TUPLE_ELEM(4, 1, state), arg) \
      , BOOST_PP_TUPLE_ELEM(4, 2, state) \
      , BOOST_PP_TUPLE_ELEM(4, 3, state) \
    )

# define EXBOOST_PARAMETER_FUNCTION_SPLIT_ARG_deduced_required(state, arg) \
    EXBOOST_PARAMETER_FUNCTION_SPLIT_ARG_required(state, arg)

# define EXBOOST_PARAMETER_FUNCTION_SPLIT_ARG_optional(state, arg) \
    ( \
        BOOST_PP_TUPLE_ELEM(4, 0, state) \
      , BOOST_PP_TUPLE_ELEM(4, 1, state) \
      , BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(4, 2, state)) \
      , BOOST_PP_SEQ_PUSH_BACK(BOOST_PP_TUPLE_ELEM(4, 3, state), arg) \
    )

# define EXBOOST_PARAMETER_FUNCTION_SPLIT_ARG_deduced_optional(state, arg) \
    EXBOOST_PARAMETER_FUNCTION_SPLIT_ARG_optional(state, arg)

# define EXBOOST_PARAMETER_FUNCTION_SPLIT_ARG(s, state, arg) \
    BOOST_PP_CAT( \
        EXBOOST_PARAMETER_FUNCTION_SPLIT_ARG_ \
      , EXBOOST_PARAMETER_FN_ARG_QUALIFIER(arg) \
    )(state, arg)

// Returns (required_count, required, optional_count, optionals) tuple
# define EXBOOST_PARAMETER_FUNCTION_SPLIT_ARGS(args) \
    BOOST_PP_SEQ_FOLD_LEFT( \
        EXBOOST_PARAMETER_FUNCTION_SPLIT_ARG \
      , (0,BOOST_PP_SEQ_NIL, 0,BOOST_PP_SEQ_NIL) \
      , args \
    )

# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG_NAME(keyword) \
    BOOST_PP_CAT(BOOST_PP_CAT(keyword,_),type)

// Helpers used as parameters to EXBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS.
# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_TEMPLATE_ARG(r, _, arg) \
    , class EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG_NAME( \
              EXBOOST_PARAMETER_FN_ARG_KEYWORD(arg) \
      )

# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG(r, _, arg) \
    , EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG_NAME( \
              EXBOOST_PARAMETER_FN_ARG_KEYWORD(arg) \
      )& EXBOOST_PARAMETER_FN_ARG_KEYWORD(arg)

# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_PARAMETER(r, _, arg) \
    , EXBOOST_PARAMETER_FN_ARG_KEYWORD(arg)

// Produces a name for the dispatch functions.
# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(name) \
    BOOST_PP_CAT( \
        boost_param_default_ \
      , BOOST_PP_CAT(__LINE__, EXBOOST_PARAMETER_MEMBER_FUNCTION_NAME(name)) \
    )

// Helper macro used below to produce lists based on the keyword argument
// names. macro is applied to every element. n is the number of
// optional arguments that should be included.
# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS(macro, n, split_args) \
    BOOST_PP_SEQ_FOR_EACH( \
        macro \
      , ~ \
      , BOOST_PP_TUPLE_ELEM(4,1,split_args) \
    ) \
    BOOST_PP_SEQ_FOR_EACH( \
        macro \
      , ~ \
      , BOOST_PP_SEQ_FIRST_N( \
          BOOST_PP_SUB(BOOST_PP_TUPLE_ELEM(4,2,split_args), n) \
        , BOOST_PP_TUPLE_ELEM(4,3,split_args) \
        ) \
    )

// Generates a keyword | default expression.
# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_EVAL_DEFAULT(arg, tag_namespace) \
    exboost::parameter::keyword< \
        tag_namespace::EXBOOST_PARAMETER_FN_ARG_KEYWORD(arg) \
    >::instance | exboost::parameter::aux::use_default_tag()

# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_GET_ARG(arg, tag_ns) \
    EXBOOST_PARAMETER_FUNCTION_CAST( \
        args[ \
            EXBOOST_PARAMETER_FUNCTION_DEFAULT_EVAL_DEFAULT( \
                arg, tag_ns \
            ) \
        ] \
      , EXBOOST_PARAMETER_FN_ARG_PRED(arg) \
      , Args \
    )

# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_BODY(name, n, split_args, tag_namespace) \
    { \
        return EXBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(name)( \
            (ResultType(*)())0 \
          , args \
          , 0L \
            EXBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
                EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_PARAMETER \
              , n \
              , split_args \
            ) \
          , EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_GET_ARG( \
                BOOST_PP_SEQ_ELEM( \
                    BOOST_PP_SUB(BOOST_PP_TUPLE_ELEM(4,2,split_args), n) \
                  , BOOST_PP_TUPLE_ELEM(4,3,split_args) \
                ) \
              , tag_namespace \
            ) \
        ); \
    }

# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_EVAL_ACTUAL_DEFAULT(arg) \
    EXBOOST_PARAMETER_FUNCTION_CAST( \
        exboost::parameter::aux::as_lvalue(EXBOOST_PARAMETER_FN_ARG_DEFAULT(arg), 0L) \
      , EXBOOST_PARAMETER_FN_ARG_PRED(arg) \
      , Args \
    )

# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_EVAL_DEFAULT_BODY(name, n, split_args, tag_ns, const_) \
    template < \
        class ResultType \
      , class Args \
        EXBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
            EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_TEMPLATE_ARG \
          , BOOST_PP_INC(n) \
          , split_args \
        ) \
    > \
    EXBOOST_PARAMETER_MEMBER_FUNCTION_STATIC(name) \
    ResultType EXBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(name)( \
        ResultType(*)() \
      , Args const& args \
      , long \
        EXBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
            EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG \
          , BOOST_PP_INC(n) \
          , split_args \
        ) \
      , exboost::parameter::aux::use_default_tag \
    ) BOOST_PP_EXPR_IF(const_, const) \
    { \
        return EXBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(name)( \
            (ResultType(*)())0 \
          , args \
          , 0L \
            EXBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
                EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_PARAMETER \
              , BOOST_PP_INC(n) \
              , split_args \
            ) \
          , EXBOOST_PARAMETER_FUNCTION_DEFAULT_EVAL_ACTUAL_DEFAULT( \
                BOOST_PP_SEQ_ELEM( \
                    BOOST_PP_SUB(BOOST_PP_TUPLE_ELEM(4,2,split_args), BOOST_PP_INC(n)) \
                  , BOOST_PP_TUPLE_ELEM(4,3,split_args) \
                ) \
            ) \
        ); \
    }

// Produces a forwarding layer in the default evaluation machine.
//
// data is a tuple:
//
//   (name, split_args)
//
// Where name is the base name of the function, and split_args is a tuple:
//
//   (required_count, required_args, optional_count, required_args)
//


// defines the actual function body for EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION below.
# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION0(z, n, data) \
    template < \
        class ResultType \
      , class Args \
        EXBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
            EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_TEMPLATE_ARG \
          , n \
          , BOOST_PP_TUPLE_ELEM(5,1,data) \
        ) \
    > \
    EXBOOST_PARAMETER_MEMBER_FUNCTION_STATIC(BOOST_PP_TUPLE_ELEM(5,0,data)) \
    ResultType EXBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(BOOST_PP_TUPLE_ELEM(5,0,data))( \
        ResultType(*)() \
      , Args const& args \
      , int \
        EXBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
            EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG \
          , n \
          , BOOST_PP_TUPLE_ELEM(5,1,data) \
        ) \
    ) BOOST_PP_EXPR_IF(BOOST_PP_TUPLE_ELEM(5,2,data), const) \
    BOOST_PP_IF( \
        n \
      , EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_BODY \
      , ; BOOST_PP_TUPLE_EAT(4) \
    )( \
        BOOST_PP_TUPLE_ELEM(5,0,data) \
      , n \
      , BOOST_PP_TUPLE_ELEM(5,1,data) \
      , BOOST_PP_TUPLE_ELEM(5,3,data) \
    )

# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION(z, n, data) \
    BOOST_PP_IF( \
        BOOST_PP_AND( \
            BOOST_PP_NOT(n) \
          , BOOST_PP_TUPLE_ELEM(5,4,data) \
        ) \
      , BOOST_PP_TUPLE_EAT(3) \
      , EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION0 \
    )(z, n, data) \
    BOOST_PP_IF( \
        BOOST_PP_EQUAL(n, BOOST_PP_TUPLE_ELEM(4,2,BOOST_PP_TUPLE_ELEM(5,1,data))) \
      , BOOST_PP_TUPLE_EAT(5) \
      , EXBOOST_PARAMETER_FUNCTION_DEFAULT_EVAL_DEFAULT_BODY \
    )( \
        BOOST_PP_TUPLE_ELEM(5,0,data) \
      , n \
      , BOOST_PP_TUPLE_ELEM(5,1,data) \
      , BOOST_PP_TUPLE_ELEM(5,3,data) \
      , BOOST_PP_TUPLE_ELEM(5,2,data) \
    )

# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_GET_ARG(r, tag_ns, arg) \
    , EXBOOST_PARAMETER_FUNCTION_CAST( \
          args[ \
              exboost::parameter::keyword<tag_ns::EXBOOST_PARAMETER_FN_ARG_KEYWORD(arg)>::instance \
          ] \
        , EXBOOST_PARAMETER_FN_ARG_PRED(arg) \
        , Args \
      )

// Generates the function template that receives a ArgumentPack, and then
// goes on to call the layers of overloads generated by
// EXBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER.
# define EXBOOST_PARAMETER_FUNCTION_INITIAL_DISPATCH_FUNCTION(name, split_args, const_, tag_ns) \
    template <class Args> \
    typename EXBOOST_PARAMETER_FUNCTION_RESULT_NAME(name)<Args>::type \
    EXBOOST_PARAMETER_MEMBER_FUNCTION_STATIC(name) \
    EXBOOST_PARAMETER_IMPL(name)(Args const& args) BOOST_PP_EXPR_IF(const_, const) \
    { \
        return EXBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(name)( \
            (typename EXBOOST_PARAMETER_FUNCTION_RESULT_NAME(name)<Args>::type(*)())0 \
          , args \
          , 0L \
 \
            BOOST_PP_SEQ_FOR_EACH( \
                EXBOOST_PARAMETER_FUNCTION_DEFAULT_GET_ARG \
              , tag_ns \
              , BOOST_PP_TUPLE_ELEM(4,1,split_args) \
            ) \
 \
        ); \
    }

// Helper for EXBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER below.
# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER_AUX( \
    name, split_args, skip_fwd_decl, const_, tag_namespace \
  ) \
    BOOST_PP_REPEAT_FROM_TO( \
        0 \
      , BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(4, 2, split_args)) \
      , EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION \
      , (name, split_args, const_, tag_namespace, skip_fwd_decl) \
    ) \
 \
    EXBOOST_PARAMETER_FUNCTION_INITIAL_DISPATCH_FUNCTION(name, split_args, const_, tag_namespace) \
\
    template < \
        class ResultType \
      , class Args \
        EXBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
            EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_TEMPLATE_ARG \
          , 0 \
          , split_args \
        ) \
    > \
    EXBOOST_PARAMETER_MEMBER_FUNCTION_STATIC(name) \
    ResultType EXBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(name)( \
        ResultType(*)() \
      , Args const& \
      , int \
        EXBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
            EXBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG \
          , 0 \
          , split_args \
        ) \
    ) BOOST_PP_EXPR_IF(const_, const)

// Generates a bunch of forwarding functions that each extract
// one more argument.
# define EXBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER(name, args, skip_fwd_decl, const_, tag_ns) \
    EXBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER_AUX( \
        name, EXBOOST_PARAMETER_FUNCTION_SPLIT_ARGS(args), skip_fwd_decl, const_, tag_ns \
    )
/**/

// Defines the result metafunction and the parameters specialization.
# define EXBOOST_PARAMETER_FUNCTION_HEAD(result, name, tag_namespace, args)   \
      EXBOOST_PARAMETER_FUNCTION_RESULT_(result, name, args)                   \
                                                                            \
          EXBOOST_PARAMETER_FUNCTION_PARAMETERS(tag_namespace, name, args)    \
          EXBOOST_PARAMETER_FUNCTION_PARAMETERS_NAME(name);                   \

// Helper for EXBOOST_PARAMETER_FUNCTION below.
# define EXBOOST_PARAMETER_FUNCTION_AUX(result, name, tag_namespace, args)    \
    EXBOOST_PARAMETER_FUNCTION_HEAD(result, name, tag_namespace, args)         \
    EXBOOST_PARAMETER_FUNCTION_IMPL_HEAD(name); \
\
    EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS(                                  \
        result, name, args, 0                                                \
      , EXBOOST_PARAMETER_FUNCTION_FWD_COMBINATIONS(args)                      \
    )                                                                        \
                                                                             \
    EXBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER(name, args, 0, 0, tag_namespace)

// Defines a Boost.Parameter enabled function with the new syntax.
# define EXBOOST_PARAMETER_FUNCTION(result, name, tag_namespace, args)    \
    EXBOOST_PARAMETER_FUNCTION_AUX(                                       \
        result, name, tag_namespace                                      \
      , EXBOOST_PARAMETER_FLATTEN(3, 2, 3, args)                           \
    )                                                                    \
/**/

// Defines a Boost.Parameter enabled function.
# define EXBOOST_PARAMETER_BASIC_FUNCTION_AUX(result, name, tag_namespace, args)    \
    EXBOOST_PARAMETER_FUNCTION_HEAD(result, name, tag_namespace, args)        \
                                                                            \
    EXBOOST_PARAMETER_FUNCTION_IMPL_FWD(name)                                 \
                                                                            \
    EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS(                                 \
        result, name, args, 0                                               \
      , EXBOOST_PARAMETER_FUNCTION_FWD_COMBINATIONS(args)                     \
    )                                                                       \
                                                                            \
    EXBOOST_PARAMETER_FUNCTION_IMPL_HEAD(name)

# define EXBOOST_PARAMETER_BASIC_FUNCTION(result, name, tag_namespace, args)  \
    EXBOOST_PARAMETER_BASIC_FUNCTION_AUX(                                     \
        result, name, tag_namespace                                     \
      , EXBOOST_PARAMETER_FLATTEN(2, 2, 3, args)                          \
    )                                                                   \
/**/

// Defines a Boost.Parameter enabled member function.
# define EXBOOST_PARAMETER_BASIC_MEMBER_FUNCTION_AUX(result, name, tag_namespace, args, const_) \
    EXBOOST_PARAMETER_FUNCTION_HEAD(result, name, tag_namespace, args)                    \
                                                                                        \
    EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS(                                             \
        result, name, args, const_                                                      \
      , EXBOOST_PARAMETER_FUNCTION_FWD_COMBINATIONS(args)                                 \
    )                                                                                   \
                                                                                        \
    EXBOOST_PARAMETER_FUNCTION_IMPL_HEAD(name) BOOST_PP_EXPR_IF(const_, const)            \
/**/

# define EXBOOST_PARAMETER_BASIC_MEMBER_FUNCTION(result, name, tag_namespace, args) \
    EXBOOST_PARAMETER_BASIC_MEMBER_FUNCTION_AUX( \
        result, name, tag_namespace \
      , EXBOOST_PARAMETER_FLATTEN(2, 2, 3, args) \
      , 0 \
    )
/**/

# define EXBOOST_PARAMETER_BASIC_CONST_MEMBER_FUNCTION(result, name, tag_namespace, args) \
    EXBOOST_PARAMETER_BASIC_MEMBER_FUNCTION_AUX( \
        result, name, tag_namespace \
      , EXBOOST_PARAMETER_FLATTEN(2, 2, 3, args) \
      , 1 \
    )
/**/



# define EXBOOST_PARAMETER_MEMBER_FUNCTION_AUX(result, name, tag_namespace, const_, args)    \
    EXBOOST_PARAMETER_FUNCTION_HEAD(result, name, tag_namespace, args)         \
\
    EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS(                                  \
        result, name, args, const_                                           \
      , EXBOOST_PARAMETER_FUNCTION_FWD_COMBINATIONS(args)                      \
    )                                                                        \
                                                                             \
    EXBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER(name, args, 1, const_, tag_namespace)

// Defines a Boost.Parameter enabled function with the new syntax.
# define EXBOOST_PARAMETER_MEMBER_FUNCTION(result, name, tag_namespace, args)    \
    EXBOOST_PARAMETER_MEMBER_FUNCTION_AUX(                                       \
        result, name, tag_namespace, 0                                     \
      , EXBOOST_PARAMETER_FLATTEN(3, 2, 3, args)                           \
    )                                                                    \
/**/

# define EXBOOST_PARAMETER_CONST_MEMBER_FUNCTION(result, name, tag_namespace, args)    \
    EXBOOST_PARAMETER_MEMBER_FUNCTION_AUX(                                       \
        result, name, tag_namespace, 1                                     \
      , EXBOOST_PARAMETER_FLATTEN(3, 2, 3, args)                           \
    )                                                                    \
/**/

// Defines a Boost.Parameter enabled constructor.

# define EXBOOST_PARAMETER_FUNCTION_ARGUMENT(r, _, i, elem) \
    BOOST_PP_COMMA_IF(i) elem& BOOST_PP_CAT(a, i)
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR00(z, n, r, data, elem) \
    BOOST_PP_IF( \
        n \
      , EXBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_TEMPLATE_Z, BOOST_PP_TUPLE_EAT(2) \
    )(z, n) \
    BOOST_PP_EXPR_IF(BOOST_PP_EQUAL(n,1), explicit) \
    BOOST_PP_TUPLE_ELEM(6,2,data)( \
        BOOST_PP_IF( \
            n \
          , BOOST_PP_SEQ_FOR_EACH_I_R \
          , BOOST_PP_TUPLE_EAT(4) \
        )( \
            r \
          , EXBOOST_PARAMETER_FUNCTION_ARGUMENT \
          , ~ \
          , elem \
        ) \
        BOOST_PP_IF(n, EXBOOST_PARAMETER_FUNCTION_FWD_MATCH_Z, BOOST_PP_TUPLE_EAT(4))( \
            z \
          , BOOST_PP_TUPLE_ELEM(6,3,data) \
          , BOOST_PP_CAT(constructor_parameters, __LINE__) \
          , n \
        ) \
    ) \
      : EXBOOST_PARAMETER_PARENTHESIZED_TYPE(BOOST_PP_TUPLE_ELEM(6,3,data)) ( \
            BOOST_PP_CAT(constructor_parameters, __LINE__)()( \
                BOOST_PP_ENUM_PARAMS_Z(z, n, a) \
            ) \
        ) \
    {}
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR0(r, data, elem) \
    EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR00( \
        BOOST_PP_TUPLE_ELEM(6,0,data) \
      , BOOST_PP_TUPLE_ELEM(6,1,data) \
      , r \
      , data \
      , elem \
    )
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_PRODUCT(r, product) \
    (product)
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR_ARITY_0(z, n, data) \
    EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR00( \
        z, n, BOOST_PP_DEDUCE_R() \
      , (z, n, BOOST_PP_TUPLE_REM(4) data) \
      , ~ \
    )
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR_ARITY_N(z, n, data) \
    BOOST_PP_SEQ_FOR_EACH( \
        EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR0 \
      , (z, n, BOOST_PP_TUPLE_REM(4) data) \
      , BOOST_PP_SEQ_FOR_EACH_PRODUCT( \
            EXBOOST_PARAMETER_FUNCTION_FWD_PRODUCT \
          , BOOST_PP_SEQ_FIRST_N( \
                n, BOOST_PP_TUPLE_ELEM(4,2,data) \
            ) \
        ) \
    )
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR(z, n, data) \
    BOOST_PP_IF( \
        n \
      , EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR_ARITY_N \
      , EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR_ARITY_0 \
    )(z,n,data) \
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTORS0(class_,base,args,combinations,range) \
    BOOST_PP_REPEAT_FROM_TO( \
        BOOST_PP_TUPLE_ELEM(2,0,range), BOOST_PP_TUPLE_ELEM(2,1,range) \
      , EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR \
      , (class_,base,combinations,BOOST_PP_TUPLE_ELEM(2,1,range)) \
    )
/**/

# define EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTORS(class_,base,args,combinations) \
    EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTORS0( \
        class_, base, args, combinations, EXBOOST_PARAMETER_ARITY_RANGE(args) \
    )
/**/

# define EXBOOST_PARAMETER_CONSTRUCTOR_AUX(class_, base, tag_namespace, args) \
    EXBOOST_PARAMETER_FUNCTION_PARAMETERS(tag_namespace, ctor, args)          \
        BOOST_PP_CAT(constructor_parameters, __LINE__); \
\
    EXBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTORS( \
        class_, base, args \
      , EXBOOST_PARAMETER_FUNCTION_FWD_COMBINATIONS(args) \
    ) \
/**/

# define EXBOOST_PARAMETER_CONSTRUCTOR(class_, base, tag_namespace, args) \
    EXBOOST_PARAMETER_CONSTRUCTOR_AUX( \
        class_, base, tag_namespace \
      , EXBOOST_PARAMETER_FLATTEN(2, 2, 3, args) \
    )
/**/

# ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
#  define EXBOOST_PARAMETER_FUNCTION_FWD_COMBINATION(r, _, i, elem) \
    (BOOST_PP_IF( \
        EXBOOST_PARAMETER_FUNCTION_IS_KEYWORD_QUALIFIER( \
            EXBOOST_PARAMETER_FN_ARG_NAME(elem) \
        ) \
      , (const ParameterArgumentType ## i)(ParameterArgumentType ## i) \
      , (const ParameterArgumentType ## i) \
    ))
// No partial ordering. This feature doesn't work.
# else
#  define EXBOOST_PARAMETER_FUNCTION_FWD_COMBINATION(r, _, i, elem) \
    (BOOST_PP_IF( \
        EXBOOST_PARAMETER_FUNCTION_IS_KEYWORD_QUALIFIER( \
            EXBOOST_PARAMETER_FN_ARG_NAME(elem) \
        ) \
      , (ParameterArgumentType ## i) \
      , (const ParameterArgumentType ## i) \
    ))
# endif

# define EXBOOST_PARAMETER_FUNCTION_FWD_COMBINATIONS(args) \
    BOOST_PP_SEQ_FOR_EACH_I(EXBOOST_PARAMETER_FUNCTION_FWD_COMBINATION, ~, args)

#endif // EXBOOST_PARAMETER_PREPROCESSOR_060206_HPP_EXBOOST

