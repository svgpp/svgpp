#include "full_parse_test_common.hpp"

void load_filter(rapidxml_ns::xml_node<char> const * svg_element)
{
  Context context;
  document_traversal<
    viewport_policy<policy::viewport::raw>,
    text_events_policy<TextEventsPolicy>,
    transform_events_policy<TransformEventsPolicy>,
    path_events_policy<PathEventsPolicy>,
    ignored_elements<boost::mpl::set0<> >,
    ignored_attributes<boost::mpl::set<
      tag::attribute::cursor,
      tag::attribute::font,
      tag::attribute::values,
      tag::attribute::keyTimes,
      tag::attribute::keySplines
    >::type>
  >::load_expected_element(svg_element, context, tag::element::filter());
}