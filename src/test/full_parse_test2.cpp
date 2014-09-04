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
      tag::attribute::font,
      tag::attribute::values,
      tag::attribute::keyTimes,
      tag::attribute::keySplines
    >::type>
  >::load_referenced_element<
    expected_elements<boost::mpl::set1<tag::element::filter> >
  >::load(svg_element, context);
}