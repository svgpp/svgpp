// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/detail/adapt_context.hpp>
#include <boost/parameter.hpp>
#include <svgpp/template_parameters.hpp>

namespace svgpp { namespace detail 
{

template<class Context, SVGPP_TEMPLATE_ARGS>
struct value_parser_parameters
{
private:
  typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::number_type>
    , boost::parameter::optional<tag::error_policy>
  >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;

public:
  typedef typename detail::unwrap_context<Context, tag::number_type>::template bind<args>::type number_type;

  typedef typename detail::unwrap_context<Context, tag::value_events_policy> value_events_context;
  typedef typename value_events_context::template bind<args>::type value_events_policy;

  typedef typename detail::unwrap_context<Context, tag::error_policy> error_policy_context;
  typedef typename error_policy_context::template bind<args>::type error_policy;
};

}}