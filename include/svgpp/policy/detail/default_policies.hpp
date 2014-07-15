#pragma once

#include <svgpp/factory/angle.hpp>
#include <svgpp/factory/color.hpp>
#include <svgpp/number_type.hpp>
#include <svgpp/policy/error.hpp>
#include <svgpp/policy/iri.hpp>
#include <svgpp/policy/length.hpp>
#include <svgpp/policy/load_path.hpp>
#include <svgpp/policy/load_transform.hpp>
#include <svgpp/policy/load_value.hpp>
#include <svgpp/policy/path.hpp>
#include <svgpp/policy/transform.hpp>
#include <svgpp/template_parameters.hpp>

namespace svgpp { namespace detail 
{

template<class Context, class PolicyTag>
struct get_default_policy;

template<class Context>
struct get_default_policy<Context, tag::number_type>
{
  typedef typename number_type_by_context<Context>::type type;
};

template<class Context>
struct get_default_policy<Context, tag::length_policy>
{
  typedef policy::length::default_policy<Context> type;
};

template<class Context>
struct get_default_policy<Context, tag::error_policy>
{
  typedef policy::error::default_policy<Context> type;
};

template<class Context>
struct get_default_policy<Context, tag::load_value_policy>
{
  typedef policy::load_value::default_policy<Context> type;
};

template<class Context>
struct get_default_policy<Context, tag::iri_policy>
{
  typedef typename policy::iri::by_context<Context>::type type;
};

template<class Context>
struct get_default_policy<Context, tag::path_policy>
{
  typedef typename policy::path::by_context<Context>::type type;
};

template<class Context>
struct get_default_policy<Context, tag::load_path_policy>
{
  typedef policy::load_path::default_policy<Context> type;
};

template<class Context>
struct get_default_policy<Context, tag::transform_policy>
{
  typedef typename policy::transform::by_context<Context>::type type;
};

template<class Context>
struct get_default_policy<Context, tag::load_transform_policy>
{
  typedef policy::load_transform::default_policy<Context> type;
};

template<class Context>
struct get_default_policy<Context, tag::angle_factory>
{
  typedef typename factory::angle::by_context<Context>::type type;
};

template<class Context>
struct get_default_policy<Context, tag::color_factory>
{
  typedef typename factory::color::by_context<Context>::type type;
};

template<class Context>
struct get_default_policy<Context, tag::icc_color_factory>
{
  typedef typename factory::icc_color::by_context<Context>::type type;
};

}}