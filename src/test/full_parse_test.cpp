#include "full_parse_test_common.hpp"

void parse(rapidxml_ns::xml_node<char> const * svg_element)
{
  Context context;
  document_traversal<
    viewport_policy<policy::viewport::raw>,
    text_events_policy<TextEventsPolicy>,
    transform_events_policy<TransformEventsPolicy>,
    path_events_policy<PathEventsPolicy>,
    ignored_elements<boost::mpl::set1<tag::element::filter> >,
    ignored_attributes<boost::mpl::set<
      tag::attribute::cursor,
      tag::attribute::font,
      tag::attribute::glyph_name,
      tag::attribute::u1,
      tag::attribute::u2,
      tag::attribute::g1,
      tag::attribute::g2,
      boost::mpl::pair<tag::element::animate, tag::attribute::values>,
      boost::mpl::pair<tag::element::animateColor, tag::attribute::values>,
      boost::mpl::pair<tag::element::animateMotion, tag::attribute::values>,
      boost::mpl::pair<tag::element::animateTransform, tag::attribute::values>,
      tag::attribute::values,
      tag::attribute::keyTimes,
      tag::attribute::keySplines,
      tag::attribute::unicode_range,
      tag::attribute::panose_1,
      tag::attribute::widths
    >::type>
  >::load_document(svg_element, context);
}