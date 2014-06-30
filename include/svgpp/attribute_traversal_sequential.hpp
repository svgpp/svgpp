#pragma once

#include <svgpp/template_parameters.hpp>
#include <svgpp/attribute_traversal_common.hpp>
#include <svgpp/context_policy.hpp>
#include <svgpp/detail/attribute_name_to_id.hpp>
#include <svgpp/detail/required_attributes_check.hpp>
#include <svgpp/xml_policy_fwd.hpp>
#include <svgpp/parser/css_style_iterator.hpp>
#include <svgpp/detail/names_dictionary.hpp>
#include <boost/mpl/if.hpp>

namespace svgpp
{

template<class RequiredAttributes, bool ParseStyleAttribute = true, SVGPP_TEMPLATE_ARGS_DEF>
class attribute_traversal_sequential
{
private:
  typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::xml_attribute_iterator_policy>,
      boost::parameter::optional<tag::error_policy>,
      boost::parameter::optional<tag::css_name_to_id_policy>
  >::bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::xml_attribute_iterator_policy, 
    detail::parameter_not_set_tag >::type xml_attribute_policy_param;
  typedef typename boost::parameter::value_type<args, tag::error_policy, 
    detail::parameter_not_set_tag>::type error_policy_param;
  typedef typename boost::parameter::value_type<args, tag::css_name_to_id_policy, 
    css_name_to_id_policy_default>::type css_name_to_id_policy;

public:
  template<class XMLAttributesIterator, class Context>
  static bool load(XMLAttributesIterator xml_attributes_iterator, Context & context)
  {
    typedef typename boost::mpl::if_<
      boost::is_same<xml_attribute_policy_param, detail::parameter_not_set_tag>,
      xml_attribute_iterator_policy<XMLAttributesIterator>,
      xml_attribute_policy_param
    >::type xml_policy;

    typedef typename boost::mpl::if_<
      boost::is_same<error_policy_param, detail::parameter_not_set_tag>,
      context_policy<tag::error_policy, Context>,
      error_policy_param
    >::type error_policy;

    detail::required_attributes_check<RequiredAttributes> required_check;
    for(; !xml_policy::is_end(xml_attributes_iterator); 
      xml_policy::advance(xml_attributes_iterator))
    {
      detail::namespace_id ns = xml_policy::get_namespace(xml_attributes_iterator);
      if (ns == detail::namespace_id::other)
        continue;
      xml_policy::attribute_name_type attribute_name = xml_policy::get_local_name(xml_attributes_iterator);
      detail::attribute_id id = detail::attribute_name_to_id(ns, attribute_name);
      switch (id)
      {
      case detail::unknown_attribute_id:
        if (!error_policy::unknown_attribute(context, xml_attributes_iterator, attribute_name, tag::source::attribute()))
          return false;
        break;
      case detail::attribute_id_style:
      {
        if (ParseStyleAttribute)
        {
          if (!load_style<xml_policy, error_policy>(xml_attributes_iterator, context))
            return false;
          break;
        }
      }
      default:
        if (!context.load_attribute(id, xml_policy::get_value(xml_attributes_iterator), tag::source::attribute()))
          return false;
        required_check(id);
      }
    }

    detail::missing_attribute_visitor<error_policy, Context> visitor(context);
    return required_check.visit_missing(visitor);
  }

private:
  template<class XMLPolicy, class ErrorPolicy, class XMLAttributesIterator, class Context>
  static bool load_style(XMLAttributesIterator const & xml_attributes_iterator, Context & context,
    typename boost::enable_if_c<ParseStyleAttribute && (true || boost::is_void<XMLAttributesIterator>::value)>::type * = NULL)
  {
    typename XMLPolicy::attribute_value_type style_value = XMLPolicy::get_value(xml_attributes_iterator);
    typedef css_style_iterator<typename boost::range_iterator<XMLPolicy::attribute_value_type>::type> css_iterator;
    for(css_iterator it(boost::begin(style_value), boost::end(style_value)); !it.eof(); ++it)
    {
      detail::attribute_id style_id = css_name_to_id_policy::find(it->first);
      if (style_id == detail::unknown_attribute_id)
      {
        if (!ErrorPolicy::unknown_attribute(context, xml_attributes_iterator, it->first, tag::source::css()))
          return false;
      }
      else
        if (!context.load_attribute(style_id, it->second, tag::source::css()))
          return false;
    }
    return true;
  }

  template<class XMLPolicy, class ErrorPolicy, class XMLAttributesIterator, class Context>
  static bool load_style(XMLAttributesIterator const &, Context &,
    typename boost::disable_if_c<ParseStyleAttribute && (true || boost::is_void<XMLAttributesIterator>::value)>::type * = NULL)
  { 
    BOOST_ASSERT(false); // Must not be called
    return true; 
  }
};

}