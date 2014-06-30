#include <svgpp/document_loader.hpp>
#include <rapidxml/rapidxml.hpp>
#include <svgpp/xml/rapidxml_policy.hpp>
//#include <boost/type_traits/integral_constant.hpp>
#include <boost/mpl/list.hpp>
#include "svg_dom.hpp"

#define BOOST_TEST_MODULE svg_dom test
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>

using namespace svgpp::dom;

namespace svgpp { 

// Adapting svgpp to our structures

template<class CharType>  
struct context_policies<SVGSVGElement<CharType> >
{
  typedef double coordinate_type;
  typedef transform_policies_raw<double> transform;
  typedef 
    boost::mpl::joint_view<
    boost::mpl::joint_view<
    boost::mpl::joint_view<
      traits::presentation_attributes,
      traits::conditional_processing_attributes>,
      traits::graphical_event_attributes>,
    boost::mpl::list<
      style_attribute_tag, // TODO: review
      baseProfile_attribute_tag>
    > ignored_attributes;
};

#if 0

using namespace svgpp::spirit;

namespace units_tag_to_SVGLength
{
  inline unsigned short convert(length_units_em_tag) { return SVGLength::SVG_LENGTHTYPE_EMS; }
  inline unsigned short convert(length_units_ex_tag) { return SVGLength::SVG_LENGTHTYPE_EXS; }
  inline unsigned short convert(length_units_px_tag) { return SVGLength::SVG_LENGTHTYPE_PX; }
  inline unsigned short convert(length_units_in_tag) { return SVGLength::SVG_LENGTHTYPE_IN; }
  inline unsigned short convert(length_units_cm_tag) { return SVGLength::SVG_LENGTHTYPE_CM; }
  inline unsigned short convert(length_units_mm_tag) { return SVGLength::SVG_LENGTHTYPE_MM; }
  inline unsigned short convert(length_units_pt_tag) { return SVGLength::SVG_LENGTHTYPE_PT; }
  inline unsigned short convert(length_units_pc_tag) { return SVGLength::SVG_LENGTHTYPE_PC; }
  inline unsigned short convert(length_units_percent_tag) { return SVGLength::SVG_LENGTHTYPE_PERCENTAGE; }
  inline unsigned short convert(length_units_none_tag) { return SVGLength::SVG_LENGTHTYPE_NUMBER; }
};

template<class UnitsTag>
inline void set_length(SVGLength & length, double value, UnitsTag tag)
{
  length.setValue(value);
  length.setUnitType(units_tag_to_SVGLength::convert(tag));
}

template<class Ch, class AttributeTag, class Value>
inline void set_value(AttributeTag, SVGElement<Ch> * element, Value const &)
{
  // Ignore value
}

template<class Ch>
inline void set_value(svgpp::attribute_baseline_shift_tag, SVGStylable<Ch> * element, SVGLength const & value)
{
  element->set_baseline_shift(value);
}

template<class Ch, class SpecialValueTag>
inline void set_value(svgpp::attribute_baseline_shift_tag, SVGStylable<Ch> * element, SpecialValueTag tag)
{
  element->set_baseline_shift(tag);
}

template<class Ch>
inline void set_value(svgpp::attribute_x_tag, SVGRectElement<Ch> * element, SVGLength const & value)
{
  element->set_x(value);
}

template<class Ch>
inline void set_value(svgpp::attribute_y_tag, SVGRectElement<Ch> * element, SVGLength const & value)
{
  element->set_y(value);
}

template<class Ch>
inline SVGSVGElement<Ch> * context_element_start(std::shared_ptr<SVGSVGElement<Ch> > & rootContext, element_svg_tag)
{
  auto element = std::make_shared<SVGSVGElement<Ch> >(nullptr);
  rootContext = element;
  return element.get();
}

template<class Ch>
inline SVGRectElement<Ch> * context_element_start(SVGElement<Ch> * context, element_rect_tag)
{
  auto element = std::make_shared<SVGRectElement<Ch> >(*context);
  context->appendChild(element);
  return element.get();
}

template<class Ch>
inline SVGSVGElement<Ch> * context_element_start(SVGElement<Ch> * context, element_svg_tag)
{
  auto element = std::make_shared<SVGSVGElement<Ch> >(context);
  context->appendChild(element);
  return element.get();
}

#endif

}

typedef boost::mpl::list<char, wchar_t> CharTypeList;

BOOST_AUTO_TEST_CASE_TEMPLATE(required_attribute, CharType, CharTypeList)
{
  static std::string const TestSVG("<svg><rect x=\"10.1\" y=\"11.1e-4\" width=\"-1982\"/></svg>");  
  rapidxml::xml_document<CharType> doc;
  std::vector<CharType> modified_xml(TestSVG.begin(), TestSVG.end());
  modified_xml.push_back(0);
  doc.parse<0>(&modified_xml[0]);  
  rapidxml::xml_node<CharType> const * svg_element = doc.first_node();
  BOOST_REQUIRE(svg_element);
  std::shared_ptr<SVGSVGElement<CharType> > context;
  BOOST_CHECK_THROW(svgpp::document_loader<rapidxml::xml_node<CharType> const *>::load(svg_element, *context), std::exception);
}

/*BOOST_AUTO_TEST_CASE_TEMPLATE(load, CharType, CharTypeList)
{
  static std::string const TestSVG("<svg><rect x=\"10.1\" y=\"11.1e-4\" width=\"-1982\" height=\"0.374\"/></svg>");  
  rapidxml::xml_document<CharType> doc;
  std::vector<CharType> modified_xml(TestSVG.begin(), TestSVG.end());
  modified_xml.push_back(0);
  doc.parse<0>(&modified_xml[0]);  
  rapidxml::xml_node<CharType> * svg_element = doc.first_node();
  BOOST_REQUIRE(svg_element);
  std::shared_ptr<SVGSVGElement<CharType> > context;
  svgpp::load_element(svg_element, context, svgpp::element_svg_tag());
  BOOST_REQUIRE(context.get());
  BOOST_CHECK_EQUAL(context->children().size(), 1);
  if (context->children().size() > 0)
  {
    SVGRectElement<CharType> const * rect = dynamic_cast<SVGRectElement<CharType> const *>(context->children()[0].get());
    BOOST_CHECK(rect);
    if (rect)
    {
      BOOST_CHECK_EQUAL(rect->x().value(), 10.1);
      BOOST_CHECK_EQUAL(rect->y().value(), 11.1e-4);
    }
  }
}*/