#pragma once

#include <svgpp/context_policy_fwd.hpp>
#include <svgpp/policy/load_transform.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(load_transform_policy)

template<class Context>
struct context_policy<tag::load_transform_policy, Context, void>: policy::load_transform::forward_to_method<Context>
{};

}