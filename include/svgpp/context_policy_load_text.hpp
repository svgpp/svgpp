#pragma once

#include <svgpp/context_policy_fwd.hpp>
#include <svgpp/policy/load_text.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(load_text_policy)

template<class Context>
struct context_policy<tag::load_text_policy, Context, void>: policy::load_text::forward_to_method<Context>
{};

}