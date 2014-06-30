#pragma once

#include <svgpp/context_policy_fwd.hpp>
#include <svgpp/policy/iri.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(iri_policy)

template<class Context>
struct context_policy<tag::iri_policy, Context, void>: policy::iri::distinguish_local
{};

}