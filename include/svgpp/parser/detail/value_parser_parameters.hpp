#pragma once

#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/policy/error.hpp>
#include <svgpp/number_type.hpp>
#include <svgpp/template_parameters.hpp>

namespace svgpp { namespace detail 
{

template<SVGPP_TEMPLATE_ARGS>
struct value_parser_parameters
{
private:
  typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::number_type>
    , boost::parameter::optional<tag::error_policy>
  >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;

public:
  template<class Context>
  struct get_number_type
  {
    typedef typename boost::parameter::value_type<args, tag::number_type, 
      typename number_type_by_context<Context>::type>::type type;
  };

  template<class Context>
  struct get_error_policy
  {
    typedef typename boost::parameter::value_type<args, tag::error_policy, 
      policy::error::default_policy<Context> >::type type;
  };
};

}}