// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/attribute_traversal/prioritized.hpp>
#include <svgpp/attribute_traversal/sequential.hpp>
#include <svgpp/policy/attribute_traversal.hpp>
#include <boost/mpl/empty.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(attribute_traversal_policy)

namespace detail
{

template<class AttributeTraversalDocumentWidePolicy, class ElementTag>
struct get_attribute_traversal_policy_for_element
{
  static const bool parse_style = AttributeTraversalDocumentWidePolicy::parse_style;
  static const bool css_hides_presentation_attribute = AttributeTraversalDocumentWidePolicy::css_hides_presentation_attribute;
  typedef typename boost::mpl::apply1<
    typename AttributeTraversalDocumentWidePolicy::get_priority_attributes_by_element,
    ElementTag
  >::type priority_attributes;
  typedef typename boost::mpl::apply1<
    typename AttributeTraversalDocumentWidePolicy::get_deferred_attributes_by_element,
    ElementTag
  >::type deferred_attributes;
  typedef typename boost::mpl::apply1<
    typename AttributeTraversalDocumentWidePolicy::get_required_attributes_by_element,
    ElementTag
  >::type required_attributes;
};

}

template<class ElementTag, SVGPP_TEMPLATE_ARGS_DEF>
struct attribute_traversal
{
private:
  typedef typename boost::parameter::parameters<
    boost::parameter::optional<tag::attribute_traversal_policy>
  >::bind<SVGPP_TEMPLATE_ARGS_PASS>::type args;
  typedef typename boost::parameter::value_type<args, tag::attribute_traversal_policy, 
    policy::attribute_traversal::default_policy>::type attribute_traversal_policy;
  typedef detail::get_attribute_traversal_policy_for_element<attribute_traversal_policy, ElementTag> 
    element_attribute_traversal_policy;

public:
  typedef typename boost::mpl::if_c<
      (element_attribute_traversal_policy::parse_style
        && element_attribute_traversal_policy::css_hides_presentation_attribute)
      || !boost::mpl::empty<typename element_attribute_traversal_policy::priority_attributes>::value
      || !boost::mpl::empty<typename element_attribute_traversal_policy::deferred_attributes>::value,
    attribute_traversal_prioritized<
      element_attribute_traversal_policy, 
      SVGPP_TEMPLATE_ARGS_PASS
    >,
    attribute_traversal_sequential<
      typename element_attribute_traversal_policy::required_attributes, 
      element_attribute_traversal_policy::parse_style, 
      SVGPP_TEMPLATE_ARGS_PASS
    >
  >::type type;
};

}
