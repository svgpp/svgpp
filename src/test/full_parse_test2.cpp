#include "full_parse_test_common.hpp"

void load_filter(rapidxml_ns::xml_node<char> const * svg_element)
{
  Context context;
  document_traversal<
    viewport_policy<policy::viewport::raw>,
    load_text_policy<LoadTextPolicy>,
    load_transform_policy<LoadTransformPolicy>,
    load_path_policy<LoadPathPolicy>,
    ignored_elements<boost::mpl::set0<> >,
    ignored_attributes<boost::mpl::set<
      tag::attribute::cursor,
      tag::attribute::font_size,
      /*boost::mpl::pair<tag::element::animate, tag::attribute::values>,
      boost::mpl::pair<tag::element::animateColor, tag::attribute::values>,
      boost::mpl::pair<tag::element::animateMotion, tag::attribute::values>,
      boost::mpl::pair<tag::element::animateTransform, tag::attribute::values>,*/
      tag::attribute::values,
      tag::attribute::keyTimes,
      tag::attribute::keySplines
    >::type>
  >::load_referenced_element<
    expected_elements<boost::mpl::set1<tag::element::filter> >
  >::load(svg_element, context);
}