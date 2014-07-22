// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/traits/element_required_attributes.hpp>

namespace svgpp { namespace policy { namespace attribute_traversal
{

struct raw
{
  static const bool css_hides_presentation_attribute = false;

  typedef boost::mpl::always<boost::mpl::empty_sequence> get_priority_attributes_by_element;
  typedef boost::mpl::always<boost::mpl::empty_sequence> get_deferred_attributes_by_element;
  typedef boost::mpl::always<boost::mpl::empty_sequence> get_required_attributes_by_element;
};

struct default_policy
{
  static const bool css_hides_presentation_attribute = true;

  typedef boost::mpl::always<boost::mpl::empty_sequence> get_priority_attributes_by_element;
  typedef boost::mpl::always<boost::mpl::empty_sequence> get_deferred_attributes_by_element;
  typedef boost::mpl::quote1<traits::element_required_attributes> get_required_attributes_by_element;

  // To be able to start drawing paths instantly as points parsed, we defer
  // path/@d, polygon/@points and polyline/@points attributes processing till transform and
  // paint are ready

  /*typedef boost::mpl::map<
    boost::mpl::pair<tag::element::path,      boost::mpl::vector1<tag::attribute::d> >,
    boost::mpl::pair<tag::element::polyline,  boost::mpl::vector1<tag::attribute::points> >,
    boost::mpl::pair<tag::element::polygon,   boost::mpl::vector1<tag::attribute::points> >
  > deferred_attributes_by_element;*/

  /*template<class ElementTag>
  struct attributes_load_order
  {
    typedef boost::mpl::joint_view<
      typename boost::mpl::if_<
        boost::is_same<ElementTag, tag::element::svg>,
        typename boost::mpl::joint_view<traits::viewport_attributes, 
          boost::mpl::single_view<traversal_detail::after_viewport_attributes_op_tag> >::type,
        boost::mpl::empty_sequence>::type,
      traits::font_selection_attributes
    > priority_operations;
  };*/
};

}}}