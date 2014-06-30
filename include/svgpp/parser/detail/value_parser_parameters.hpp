#pragma once

#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/context_policy.hpp>
#include <svgpp/template_parameters.hpp>

namespace svgpp { namespace detail 
{

template<SVGPP_TEMPLATE_ARGS>
struct value_parser_parameters
{
  typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::number_type>
    , boost::parameter::optional<tag::error_policy>
  >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;

private:
  typedef typename boost::parameter::value_type<args, tag::number_type, 
    parameter_not_set_tag>::type number_type_param;

  typedef typename boost::parameter::value_type<args, tag::error_policy, 
    parameter_not_set_tag>::type error_policy_param;

  struct get_number_type_from_policy
  {
    template<class Context>
    struct apply
    {
      typedef typename context_policy<tag::number_type, Context>::type type;
    };
  };

public:
  typedef typename boost::mpl::if_<
    boost::is_same<number_type_param, parameter_not_set_tag>,
    get_number_type_from_policy,
    boost::mpl::always<number_type_param>
  >::type get_number_type;

  typedef typename boost::mpl::if_<
    boost::is_same<error_policy_param, parameter_not_set_tag>,
    boost::mpl::bind<boost::mpl::quote3<context_policy>, tag::error_policy, boost::mpl::_1, void>,
    boost::mpl::always<error_policy_param>
  >::type get_error_policy;
};

}}