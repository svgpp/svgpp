#pragma once

#include <svgpp/context_policy_fwd.hpp>
#include <svgpp/unitless_angle_factory.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(angle_factory)

template<class Context>
struct context_policy<tag::angle_factory, Context, void>: unitless_angle_factory<double>
{};

}