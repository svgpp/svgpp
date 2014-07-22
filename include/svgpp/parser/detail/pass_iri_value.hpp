// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/move/utility.hpp>
#include <boost/preprocessor.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility.hpp>

namespace svgpp { namespace detail
{

template<class LoadPolicy>
struct pass_iri_value_proxy
{
#ifndef BOOST_NO_CXX11_VARIADIC_TEMPLATES
  template<class Context, class AttributeTag, class IRI, class... Args>
  static void set(Context & context, AttributeTag tag, IRI const & iri, BOOST_FWD_REF(Args)... args)
  {
    if (boost::begin(iri) != boost::end(iri) && *boost::begin(iri) == '#') // TODO: is portable?
      LoadPolicy::set(context, tag, 
        tag::iri_fragment(), 
        IRI(boost::next(boost::begin(iri)), boost::end(iri)), boost::forward<Args>(args)...);
    else
      LoadPolicy::set(context, tag, iri, boost::forward<Args>(args)...);
  }
#else
# define BOOST_PP_LOCAL_MACRO(n)                                                                     \
  template<class Context, class AttributeTag, class IRI BOOST_PP_ENUM_TRAILING_PARAMS(n, class Arg)> \
  static void set(Context & context, AttributeTag tag, IRI const & iri                               \
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(n, const Arg, & arg))                                       \
  {                                                                                                  \
    if (boost::begin(iri) != boost::end(iri) && *boost::begin(iri) == '#')                           \
      LoadPolicy::set(context, tag,                                                                  \
      tag::iri_fragment(),                                                                           \
      IRI(boost::next(boost::begin(iri)), boost::end(iri)) BOOST_PP_ENUM_TRAILING_PARAMS(n, arg));   \
    else                                                                                             \
      LoadPolicy::set(context, tag, iri BOOST_PP_ENUM_TRAILING_PARAMS(n, arg));                      \
  }
# define BOOST_PP_LOCAL_LIMITS (0, 2)
# include BOOST_PP_LOCAL_ITERATE()
#endif
};

template<class LoadPolicy, class IRIPolicy, class Enable = void>
struct load_value_with_iri_policy;

template<class LoadPolicy, class IRIPolicy>
struct load_value_with_iri_policy<
  LoadPolicy, 
  IRIPolicy, 
  typename boost::disable_if_c<IRIPolicy::distinguish_local_references>::type
>
{
  typedef LoadPolicy type;
};

template<class LoadPolicy, class IRIPolicy>
struct load_value_with_iri_policy<
  LoadPolicy, 
  IRIPolicy, 
  typename boost::enable_if_c<IRIPolicy::distinguish_local_references>::type
>
{
  typedef pass_iri_value_proxy<LoadPolicy> type;
};

}}