#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>
#include <svgpp/svgpp.hpp>

using namespace svgpp;

typedef rapidxml_ns::xml_node<> const * xml_element_t;

class Context
{
public:
  void on_enter_element(tag::element::any)
  {}

  void on_exit_element()
  {}

  void transform_matrix(const boost::array<double, 6> & matrix)
  {}

  void path_move_to(double x, double y, tag::coordinate::absolute)
  {}

  void path_line_to(double x, double y, tag::coordinate::absolute)
  {}

  void path_cubic_bezier_to(
    double x1, double y1,
    double x2, double y2,
    double x, double y,
    tag::coordinate::absolute)
  {}

  void path_quadratic_bezier_to(
    double x1, double y1,
    double x, double y,
    tag::coordinate::absolute)
  {}

  void path_elliptical_arc_to(
    double rx, double ry, double x_axis_rotation,
    bool large_arc_flag, bool sweep_flag,
    double x, double y,
    tag::coordinate::absolute)
  {}

  void path_close_subpath()
  {}

  void path_exit()
  {}
};

typedef 
  boost::mpl::set<
    // SVG Structural Elements
    tag::element::svg,
    tag::element::g,
    // SVG Shape Elements
    tag::element::circle,
    tag::element::ellipse,
    tag::element::line,
    tag::element::path,
    tag::element::polygon,
    tag::element::polyline,
    tag::element::rect
  >::type processed_elements_t;

typedef
  boost::mpl::insert<
    traits::shapes_attributes_by_element,
    tag::attribute::transform
  >::type processed_attributes_t;

void loadSvg(xml_element_t xml_root_element)
{
  Context context;
  document_traversal<
    processed_elements<processed_elements_t>,
    processed_attributes<processed_attributes_t>
  >::load_document(xml_root_element, context);
}

int main()
{
#define TEXT(x) #x
  char text[] = 
    TEXT(<svg xmlns="http://www.w3.org/2000/svg">)
    TEXT( <ellipse cx="50" cy="25" rx="20" ry="100" transform="rotate(45 50 25)"/>)
    TEXT(</svg>);

  rapidxml_ns::xml_document<> doc;    // character type defaults to char
  try
  {
    doc.parse<0>(text);  
    if (rapidxml_ns::xml_node<> * svg_element = doc.first_node("svg"))
    {
      loadSvg(svg_element);
    }
  }
  catch (std::exception const & e)
  {
    std::cerr << "Error loading SVG: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
