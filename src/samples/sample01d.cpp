#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>
#include <svgpp/svgpp.hpp>

using namespace svgpp;

typedef rapidxml_ns::xml_node<> const * xml_element_t;

class BaseContext
{
public:
  void on_exit_element()
  {}

  void transform_matrix(const boost::array<double, 6> & matrix)
  {}

  void set_viewport(double viewport_x, double viewport_y, double viewport_width, double viewport_height)
  {}

  void disable_rendering()
  {}
};

class ShapeContext: public BaseContext
{
public:
  ShapeContext(BaseContext const & parent)
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

struct ChildContextFactories
{
  template<class ParentContext, class ElementTag, class Enable = void>
  struct apply
  {
    // Default definition handles "svg" and "g" elements
    typedef factory::context::on_stack<BaseContext> type;
  };
};

// This specialization handles all shape elements (elements from traits::shape_elements sequence)
template<class ElementTag>
struct ChildContextFactories::apply<BaseContext, ElementTag,
  typename boost::enable_if<boost::mpl::has_key<traits::shape_elements, ElementTag> >::type>
{
  typedef factory::context::on_stack<ShapeContext> type;
};

typedef 
  boost::mpl::fold<
    traits::shape_elements,
    boost::mpl::set<
      tag::element::svg,
      tag::element::g
    >::type,
    boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
  >::type processed_elements_t;

// Joining some sequences from traits namespace with chosen attributes
typedef 
  boost::mpl::fold<
    boost::mpl::protect<
      boost::mpl::joint_view<
        traits::shapes_attributes_by_element, 
        traits::viewport_attributes
      >
    >,
    boost::mpl::set<
      tag::attribute::transform
    >::type,
    boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
  >::type processed_attributes_t;

void loadSvg(xml_element_t xml_root_element)
{
  BaseContext context;
  document_traversal<
    processed_elements<processed_elements_t>,
    processed_attributes<processed_attributes_t>,
    viewport_policy<policy::viewport::as_transform>,
    context_factories<ChildContextFactories>
  >::load_document(xml_root_element, context);
}

int main()
{
  char text[] = "<svg/>";
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
