#pragma once

#include <svgpp/policy/detail/default_policies.hpp>

namespace svgpp { namespace detail 
{

template<class OriginalContext, class Parameters>
struct bind_context_parameters_wrapper
{
  bind_context_parameters_wrapper(
    OriginalContext & original_context)
    : original_context(original_context)
  {}

  OriginalContext & original_context;
};

template<class Parameters, class OriginalContext>
bind_context_parameters_wrapper<OriginalContext, Parameters>
  bind_context_parameters(OriginalContext & context)
{
  return bind_context_parameters_wrapper<OriginalContext, Parameters>(context);
}

// TODO: specialize to copy references from previous adapter
template<class OriginalContext, class AdaptedPolicyTag, class AdaptedPolicy>
struct adapted_policy_context_wrapper
{
  adapted_policy_context_wrapper(
    OriginalContext & original_context)
    : original_context(original_context)
  {}

  OriginalContext & original_context;
};

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

  OriginalContext & original_context;
  AdaptedContext & adapted_context;
};

template<class AdaptedPolicyTag, class AdaptedPolicy, class OriginalContext, class AdapterContext>
adapted_context_wrapper<
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
adapted_context_wrapper<
  OriginalContext, 
  AdapterContext, 
  tag::load_value_policy, 
  policy::load_value::default_policy<AdapterContext> 
> 
adapt_context_load_value(OriginalContext & original_context, AdapterContext & adapter_context)
{
  return adapted_context_wrapper<
    OriginalContext, 
    AdapterContext, 
    tag::load_value_policy, 
    policy::load_value::default_policy<AdapterContext> 
  >(original_context, adapter_context);
}

template<class AdapterLoadValuePolicy, class OriginalContext, class AdapterContext>
adapted_context_wrapper<
  OriginalContext, 
  AdapterContext, 
  tag::load_value_policy, 
  AdapterLoadValuePolicy
> 
adapt_context_load_value2(OriginalContext & original_context, AdapterContext & adapter_context)
{
  return adapted_context_wrapper<
    OriginalContext, 
    AdapterContext, 
    tag::load_value_policy, 
    AdapterLoadValuePolicy
  >(original_context, adapter_context);
}

struct undefined_policy_stub;

template<class Context, class PolicyTag>
struct unwrap_context
{
  typedef Context type;

  static Context & get(Context & context)
  {
    return context;
  }

// TODO: rename bind to get_policy
  template<class Parameters>
  struct bind
  {
    typedef typename boost::parameter::value_type<Parameters, PolicyTag, 
      typename get_default_policy<Context, PolicyTag>::type>::type type;
  };

  typedef undefined_policy_stub policy;
};

template<class Context, class Parameters, class PolicyTag>
struct unwrap_context<bind_context_parameters_wrapper<Context, Parameters>, PolicyTag>
  : unwrap_context<Context, PolicyTag>
{
  static typename unwrap_context<Context, PolicyTag>::type & get(
    bind_context_parameters_wrapper<Context, Parameters> & wrapper)
  {
    return unwrap_context<Context, PolicyTag>::get(wrapper.original_context);
  }

  typedef typename unwrap_context<Context, PolicyTag>::template bind<Parameters>::type policy;
};

template<class OriginalContext, class AdaptedContext, class AdaptedPolicyTag, class AdaptedPolicy>
struct unwrap_context<adapted_context_wrapper<OriginalContext, AdaptedContext, AdaptedPolicyTag, AdaptedPolicy>, AdaptedPolicyTag>
{
  typedef AdaptedContext type;

  static AdaptedContext & get(adapted_context_wrapper<OriginalContext, AdaptedContext, AdaptedPolicyTag, AdaptedPolicy> & wrapper)
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

  static type & get(adapted_context_wrapper<OriginalContext, AdaptedContext, AdaptedPolicyTag, AdaptedPolicy> & wrapper)
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

}}