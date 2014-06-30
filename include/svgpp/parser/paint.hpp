#pragma once

#include <svgpp/config.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/detail/pass_iri_value.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/grammar/color_optional_icc_color.hpp>
#include <svgpp/parser/grammar/iri.hpp>
#include <svgpp/context_policy_color_factory.hpp>
#include <svgpp/context_policy_load_value.hpp>
#include <svgpp/context_policy_iri.hpp>

namespace svgpp
{

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::paint, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    PropertySource)
  {
    namespace qi = boost::spirit::qi;
    namespace phx = boost::phoenix;
    using detail::character_encoding_namespace::space;

    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::color_factory>,
      boost::parameter::optional<tag::icc_color_factory>,
      boost::parameter::optional<tag::iri_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename boost::parameter::value_type<args2_t, tag::color_factory, 
      context_policy<tag::color_factory, Context> >::type color_factory_t;
    typedef typename boost::parameter::value_type<args2_t, tag::icc_color_factory, 
      context_policy<tag::icc_color_factory, Context> >::type icc_color_factory_t;
    typedef typename boost::parameter::value_type<args2_t, tag::iri_policy, 
      context_policy<tag::iri_policy, Context> >::type iri_policy_t;

    SVGPP_STATIC_IF_SAFE const color_optional_icc_color_grammar<
      PropertySource, iterator_t, color_factory_t, icc_color_factory_t> color_optional_icc_color;
    boost::tuple<typename color_factory_t::color_type, boost::optional<typename icc_color_factory_t::icc_color_type> > 
      color;
    SVGPP_STATIC_IF_SAFE const funciri_grammar<iterator_t> funciri_rule;
    typename boost::iterator_range<iterator_t> iri;
    static const int opt_not_set = 0, opt_none = 1, opt_currentColor = 2, opt_inherit = 3, opt_color = 4, opt_funciri = 5;
    int main_option, funciri_suboption = opt_not_set;

    qi::rule<iterator_t> rule = 
        qi::lit("none")           [phx::ref(main_option) = opt_none]
      | qi::lit("currentColor")   [phx::ref(main_option) = opt_currentColor]
      | qi::lit("inherit")        [phx::ref(main_option) = opt_inherit]
      | color_optional_icc_color  [phx::ref(color) = qi::_1, phx::ref(main_option) = opt_color]
      | ( funciri_rule [phx::ref(iri) = qi::_1]
          >> - ( +space
                  >> ( qi::lit("none")          [phx::ref(funciri_suboption) = opt_none]
                     | qi::lit("currentColor")  [phx::ref(funciri_suboption) = opt_currentColor]
                     | color_optional_icc_color [phx::ref(color) = qi::_1, phx::ref(funciri_suboption) = opt_color]
                     )
               )
        ) [phx::ref(main_option) = opt_funciri];

    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    if (qi::parse(it, end, rule) && it == end)
    {
      typedef context_policy<tag::load_value_policy, Context> load_value_policy_t;
      typedef detail::load_value_with_iri_policy<load_value_policy_t, iri_policy_t>::type 
        load_value_with_iri_policy_t;

      switch (main_option)
      {
      case opt_none:
        load_value_policy_t::set(context, tag, tag::value::none());
        break;
      case opt_currentColor:
        load_value_policy_t::set(context, tag, tag::value::currentColor());
        break;
      case opt_inherit:
        load_value_policy_t::set(context, tag, tag::value::inherit());
        break;
      case opt_color:
        if (color.get<1>())
          load_value_policy_t::set(context, tag, color.get<0>(), *color.get<1>());
        else
          load_value_policy_t::set(context, tag, color.get<0>());
        break;
      case opt_funciri:
        switch (funciri_suboption)
        {
        case opt_not_set:
          load_value_with_iri_policy_t::set(context, tag, iri);
          break;
        case opt_none:
          load_value_with_iri_policy_t::set(context, tag, iri, tag::value::none());
          break;
        case opt_currentColor:
          load_value_with_iri_policy_t::set(context, tag, iri, tag::value::currentColor());
          break;
        case opt_color:
          if (color.get<1>())
            load_value_with_iri_policy_t::set(context, tag, iri, color.get<0>(), *color.get<1>());
          else
            load_value_with_iri_policy_t::set(context, tag, iri, color.get<0>());
          break;
        }
        break;
      }
      return true;
    }
    else
    {
      typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
      return args_t::get_error_policy::template apply<Context>::type::parse_failed(context, tag, attribute_value);
    }
  }
};

}