#pragma once

#include <boost/utility/enable_if.hpp>
#include <boost/utility.hpp>

namespace svgpp { namespace detail
{

template<class LoadPolicy>
struct pass_iri_value_proxy
{
#ifndef BOOST_NO_CXX11_VARIADIC_TEMPLATES
  template<class Context, class AttributeTag, class IRI, class... Args>
  static void set(Context & context, AttributeTag tag, IRI const & iri, Args... args)
  {
    if (boost::begin(iri) != boost::end(iri) && *boost::begin(iri) == '#') // TODO: is portable?
      LoadPolicy::set(context, tag, 
        tag::iri_fragment(), 
        IRI(boost::next(boost::begin(iri)), boost::end(iri)), args...);
    else
      LoadPolicy::set(context, tag, iri, args...);
  }
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