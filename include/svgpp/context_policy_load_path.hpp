#pragma once

#include <svgpp/context_policy_fwd.hpp>
#include <svgpp/policy/load_path.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(load_path_policy)

template<class Context>
struct context_policy<tag::load_path_policy, Context, void>: policy::load_path::forward_to_method<Context>
{};

}