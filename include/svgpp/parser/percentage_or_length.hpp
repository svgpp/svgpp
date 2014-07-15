#pragma once

#include <svgpp/parser/length.hpp>

namespace svgpp 
{ 

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::percentage_or_length, SVGPP_TEMPLATE_ARGS_PASS>
  : value_parser<tag::type::length, SVGPP_TEMPLATE_ARGS_PASS>
{
  using value_parser<tag::type::length, SVGPP_TEMPLATE_ARGS_PASS>::parse;

  template<class AttributeTag, class Context, class AttributeValue>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::css)
  {
    typedef typename traits::length_direction_by_attribute<AttributeTag>::type direction_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    typedef detail::value_parser_parameters<Context, SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::length_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename detail::unwrap_context<Context, tag::length_policy> length_policy_context;
    typedef typename length_policy_context::bind<args2_t>::type length_policy_t;

    typename length_policy_t::length_factory_type & length_factory 
      = length_policy_t::length_factory(length_policy_context::get(context));
    SVGPP_STATIC_IF_SAFE const percentage_or_length_css_grammar<
      iterator_t, 
      typename length_policy_t::length_factory_type, 
      direction_t
    > length_grammar;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    typename length_policy_t::length_factory_type::length_type value;
    if (boost::spirit::qi::parse(it, end, length_grammar(boost::phoenix::ref(length_factory)), value) 
      && it == end)
    {
      args_t::load_value_policy::set(args_t::load_value_context::get(context), tag, value);
      return true;
    }
    else
    {
      return args_t::error_policy::parse_failed(args_t::error_policy_context::get(context), tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::list_of<tag::type::percentage_or_length>, SVGPP_TEMPLATE_ARGS_PASS>
  : value_parser<tag::type::list_of<tag::type::length>, SVGPP_TEMPLATE_ARGS_PASS>
{
  using value_parser<tag::type::list_of<tag::type::length>, SVGPP_TEMPLATE_ARGS_PASS>::parse;

  template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::css property_source)
  {
    typedef typename traits::length_direction_by_attribute<AttributeTag>::type direction_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    return parseT<
      boost::mpl::bind<
        boost::mpl::quote5<percentage_or_length_css_grammar>, 
        iterator_t, boost::mpl::_1, direction_t, boost::mpl::_2
      >,
      AttributeTag, Context, AttributeValue, tag::source::css>
      (tag, context, attribute_value, property_source);
  }
};

}