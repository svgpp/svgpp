#pragma once

#include <svgpp/context_policy_iri.hpp>
#include <svgpp/context_policy_load_path.hpp>
#include <svgpp/context_policy_load_text.hpp>
#include <svgpp/context_policy_load_transform.hpp>
#include <svgpp/context_policy_load_value.hpp>
#include <svgpp/context_policy_color_factory.hpp>
#include <svgpp/policy/error.hpp>

namespace svgpp
{

template<class Context>
struct context_policy<tag::number_type, Context, void>
{
  typedef double type;
};

template<class Context>
struct context_policy<tag::error_policy, Context, void>: policy::error::raise_exception<Context>
{};

}