#pragma once

#include <svgpp/context_policy_fwd.hpp>
#include <svgpp/integer_color_factory.hpp>
#include <svgpp/skip_icc_color_factory.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(color_factory)
BOOST_PARAMETER_TEMPLATE_KEYWORD(icc_color_factory)

template<class Context>
struct context_policy<tag::color_factory, Context, void>: integer_color_factory<>
{};

template<class Context>
struct context_policy<tag::icc_color_factory, Context, void>: skip_icc_color_factory
{};

}