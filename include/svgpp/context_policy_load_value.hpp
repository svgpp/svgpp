#pragma once

#include <svgpp/context_policy_fwd.hpp>
#include <svgpp/policy/load_value.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(load_value_policy)

template<class Context>
struct context_policy<tag::load_value_policy, Context, void>: policy::load_value::default_policy<Context>
{};

}