#include <svgpp/document_traversal.hpp>
#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>

#include <gtest/gtest.h>

#define TEXT(x) #x

namespace
{
  class Context
  {
  public:
    template<class Points>
    void set(svgpp::tag::attribute::points, Points const &)
    {}

    std::string str() const 
    {
      return log_.str();
    }

    void on_enter_element(svgpp::tag::element::any const &) {}
    void on_exit_element() const {}

    void path_move_to(double x, double y, svgpp::tag::coordinate::absolute) {}
    void path_line_to(double x, double y, svgpp::tag::coordinate::absolute) {}
    void path_exit() {}

    void marker(svgpp::marker_vertex v, double x, double y, double directionality, unsigned marker_index)
    {
      log_ << "Marker:" << v << " " << x << "," << y << " " << directionality << " " << marker_index << ";";
    }

  protected:
    std::ostringstream log_;
  };

  char const xml1[] = 
    TEXT(<svg xmlns="http://www.w3.org/2000/svg"><polyline points="0, 0 200, 200 400,0 200, -200 0, 0"/></svg>)
    ;
}

TEST(PolylineMarkers, Test1)
{
  std::vector<char> modified_xml(xml1, xml1 + strlen(xml1) + 1);
  rapidxml_ns::xml_document<char> doc;
  doc.parse<0>(&modified_xml[0]);  
  rapidxml_ns::xml_node<char> const * svg_element = doc.first_node();
  ASSERT_TRUE(svg_element != NULL);
  Context context;
  EXPECT_TRUE((
    svgpp::document_traversal<
      svgpp::processed_elements<
        boost::mpl::set<
          svgpp::tag::element::svg, 
          svgpp::tag::element::polyline
        >::type
      >,
      svgpp::processed_attributes<
        boost::mpl::set<
          svgpp::tag::attribute::points
        >::type
      >,
      svgpp::markers_policy<svgpp::policy::markers::calculate_always>
    >::load_document(svg_element, context)));

  {
    Context sample_context;
    sample_context.marker(svgpp::marker_mid, 200, 200, 0, 1);
    sample_context.marker(svgpp::marker_mid, 400, 0, -90 * boost::math::constants::degree<double>(), 2);
    sample_context.marker(svgpp::marker_mid, 200, -200, -180 * boost::math::constants::degree<double>(), 3);
    sample_context.marker(svgpp::marker_end, 0, 0, 135 * boost::math::constants::degree<double>(), 4);
    sample_context.marker(svgpp::marker_start, 0, 0, 45 * boost::math::constants::degree<double>(), 0);
    EXPECT_EQ(sample_context.str(), context.str());
  }
}