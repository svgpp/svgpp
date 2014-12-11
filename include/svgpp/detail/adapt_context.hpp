// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/policy/detail/default_policies.hpp>

namespace svgpp { namespace detail 
{

template<class Context>
struct is_context_inexpensively_copyable: boost::mpl::false_
{};

template<class Context>
struct is_context_inexpensively_copyable<const Context>: is_context_inexpensively_copyable<Context>
{};

template<class Context, class Enable = void>
struct context_copy_or_reference
{
  typedef Context & type;
};

template<class Context>
struct context_copy_or_reference<Context, 
  typename boost::enable_if<is_context_inexpensively_copyable<Context> >::type>
{
  typedef Context type;
};

template<class OriginalContext, class Parameters>
struct bind_context_parameters_wrapper
{
  bind_context_parameters_wrapper(
    OriginalContext & original_context)
    : original_context(original_context)
  {}

  typename context_copy_or_reference<OriginalContext>::type original_context;

  BOOST_DELETED_FUNCTION(bind_context_parameters_wrapper& operator= (bind_context_parameters_wrapper const&))
};

template<class OriginalContext, class Parameters>
struct is_context_inexpensively_copyable<bind_context_parameters_wrapper<OriginalContext, Parameters> >
  : boost::mpl::true_
{};

template<class Parameters, class OriginalContext>
const bind_context_parameters_wrapper<OriginalContext, Parameters>
  bind_context_parameters(OriginalContext & context)
{
  return bind_context_parameters_wrapper<OriginalContext, Parameters>(context);
}

template<class OriginalContext, class AdaptedPolicyTag, class AdaptedPolicy>
struct adapted_policy_context_wrapper
{
  adapted_policy_context_wrapper(
    OriginalContext & original_context)
    : original_context(original_context)
  {}

  typename context_copy_or_reference<OriginalContext>::type original_context;

  BOOST_DELETED_FUNCTION(adapted_policy_context_wrapper& operator= (adapted_policy_context_wrapper const&))
};

template<class OriginalContext, class AdaptedPolicyTag, class AdaptedPolicy>
struct is_context_inexpensively_copyable<
  adapted_policy_context_wrapper<OriginalContext, AdaptedPolicyTag, AdaptedPolicy> >
  : boost::mpl::true_
{};

template<class AdaptedPolicyTag, class AdaptedPolicy, class OriginalContext>
adapted_policy_context_wrapper<
  OriginalContext, 
  AdaptedPolicyTag, 
  AdaptedPolicy
> 
adapt_context_policy(OriginalContext & original_context)
{
  return adapted_policy_context_wrapper<
    OriginalContext, 
    AdaptedPolicyTag, 
    AdaptedPolicy
  > (original_context);
}

template<class OriginalContext, class AdaptedContext, class AdaptedPolicyTag, class AdaptedPolicy>
struct adapted_context_wrapper
{
  adapted_context_wrapper(
    OriginalContext & original_context,
    AdaptedContext & adapted_context)
    : original_context(original_context)
    , adapted_context(adapted_context)
  {}

  typename context_copy_or_reference<OriginalContext>::type original_context;
  typename context_copy_or_reference<AdaptedContext>::type adapted_context;

  BOOST_DELETED_FUNCTION(adapted_context_wrapper& operator= (adapted_context_wrapper const&))
};

template<class OriginalContext, class AdaptedContext, class AdaptedPolicyTag, class AdaptedPolicy>
struct is_context_inexpensively_copyable<
  adapted_context_wrapper<OriginalContext, AdaptedContext, AdaptedPolicyTag, AdaptedPolicy> >
  : boost::mpl::true_
{};

template<class AdaptedPolicyTag, class AdaptedPolicy, class OriginalContext, class AdapterContext>
const adapted_context_wrapper<
  OriginalContext, 
  AdapterContext, 
  AdaptedPolicyTag, 
  AdaptedPolicy 
> 
adapt_context(OriginalContext & original_context, AdapterContext & adapter_context)
{
  return adapted_context_wrapper<
    OriginalContext, 
    AdapterContext, 
    AdaptedPolicyTag, 
    AdaptedPolicy 
  >(original_context, adapter_context);
}

template<class OriginalContext, class AdapterContext>
const adapted_context_wrapper<
  OriginalContext, 
  AdapterContext, 
  tag::value_events_policy, 
  policy::value_events::default_policy<AdapterContext> 
> 
adapt_context_value_events(OriginalContext & original_context, AdapterContext & adapter_context)
{
  return adapted_context_wrapper<
    OriginalContext, 
    AdapterContext, 
    tag::value_events_policy, 
    policy::value_events::default_policy<AdapterContext> 
  >(original_context, adapter_context);
}

template<class Context, class PolicyTag>
struct unwrap_context
{
  typedef Context type;

  static Context & get(Context & context)
  {
    return context;
  }

  template<class Parameters>
  struct bind
  {
    typedef typename boost::parameter::value_type<Parameters, PolicyTag, 
      typename get_default_policy<Context, PolicyTag>::type>::type type;
  };

  // Undefined "policy" stub may get referenced, when context is not bound to parameters 
  // via bind or bind_context_parameters_wrapper
  struct policy;
};

template<class Context, class Parameters, class PolicyTag>
struct unwrap_context<bind_context_parameters_wrapper<Context, Parameters>, PolicyTag>
{
  typedef typename unwrap_context<Context, PolicyTag>::type type;

  static typename unwrap_context<Context, PolicyTag>::type & get(
    const bind_context_parameters_wrapper<Context, Parameters> & wrapper)
  {
    return unwrap_context<Context, PolicyTag>::get(wrapper.original_context);
  }

  template<class PassedParameters>
  struct bind
  {
    typedef typename unwrap_context<Context, PolicyTag>::template bind<Parameters>::type type;
  };

  typedef typename unwrap_context<Context, PolicyTag>::template bind<Parameters>::type policy;
};

// const specialization
template<class Context, class Parameters, class PolicyTag>
struct unwrap_context<const bind_context_parameters_wrapper<Context, Parameters>, PolicyTag>
  : unwrap_context<bind_context_parameters_wrapper<Context, Parameters>, PolicyTag>
{};

template<class OriginalContext, class AdaptedPolicyTag, class AdaptedPolicy>
struct unwrap_context<adapted_policy_context_wrapper<OriginalContext, AdaptedPolicyTag, AdaptedPolicy>, AdaptedPolicyTag>
{
  struct type; // Must not be used

  template<class Parameters>
  struct bind
  {
    typedef AdaptedPolicy type;
  };

  typedef AdaptedPolicy policy;
};

template<class OriginalContext, class AdaptedPolicyTag, class AdaptedPolicy, class PolicyTag>
struct unwrap_context<adapted_policy_context_wrapper<OriginalContext, AdaptedPolicyTag, AdaptedPolicy>, PolicyTag>
{
  typedef typename unwrap_context<OriginalContext, PolicyTag>::type type;

  static type & get(const adapted_policy_context_wrapper<OriginalContext, AdaptedPolicyTag, AdaptedPolicy> & wrapper)
  {
    return unwrap_context<OriginalContext, PolicyTag>::get(wrapper.original_context);
  }

  template<class Parameters>
  struct bind
  {
    typedef typename unwrap_context<OriginalContext, PolicyTag>::template bind<Parameters>::type type;
  };

  typedef typename unwrap_context<OriginalContext, PolicyTag>::policy policy;
};

// const specialization
template<class OriginalContext, class AdaptedPolicyTag, class AdaptedPolicy, class PolicyTag>
struct unwrap_context<const adapted_policy_context_wrapper<OriginalContext, AdaptedPolicyTag, AdaptedPolicy>, PolicyTag>
  : unwrap_context<adapted_policy_context_wrapper<OriginalContext, AdaptedPolicyTag, AdaptedPolicy>, PolicyTag>
{};

template<class OriginalContext, class AdaptedContext, class AdaptedPolicyTag, class AdaptedPolicy>
struct unwrap_context<adapted_context_wrapper<OriginalContext, AdaptedContext, AdaptedPolicyTag, AdaptedPolicy>, AdaptedPolicyTag>
{
  typedef AdaptedContext type;

  static AdaptedContext & get(const adapted_context_wrapper<OriginalContext, AdaptedContext, AdaptedPolicyTag, AdaptedPolicy> & wrapper)
  {
    return wrapper.adapted_context;
  }

  template<class Parameters>
  struct bind
  {
    typedef AdaptedPolicy type;
  };

  typedef AdaptedPolicy policy;
};

// Recursively unwrapping
template<class OriginalContext, class AdaptedContext, class AdaptedPolicyTag, class AdaptedPolicy, class PolicyTag>
struct unwrap_context<adapted_context_wrapper<OriginalContext, AdaptedContext, AdaptedPolicyTag, AdaptedPolicy>, PolicyTag>
{
  typedef typename unwrap_context<OriginalContext, PolicyTag>::type type;

  static type & get(const adapted_context_wrapper<OriginalContext, AdaptedContext, AdaptedPolicyTag, AdaptedPolicy> & wrapper)
  {
    return unwrap_context<OriginalContext, PolicyTag>::get(wrapper.original_context);
  }

  template<class Parameters>
  struct bind
  {
    typedef typename unwrap_context<OriginalContext, PolicyTag>::template bind<Parameters>::type type;
  };

  typedef typename unwrap_context<OriginalContext, PolicyTag>::policy policy;
};

// const specialization
template<class OriginalContext, class AdaptedContext, class AdaptedPolicyTag, class AdaptedPolicy, class PolicyTag>
struct unwrap_context<const adapted_context_wrapper<OriginalContext, AdaptedContext, AdaptedPolicyTag, AdaptedPolicy>, PolicyTag>
  : unwrap_context<adapted_context_wrapper<OriginalContext, AdaptedContext, AdaptedPolicyTag, AdaptedPolicy>, PolicyTag>
{};

}}