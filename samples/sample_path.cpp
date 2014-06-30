#include <rapidxml/rapidxml.hpp>
#include <svgpp/xml/rapidxml.hpp>
#include <svgpp/document_traversal.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/transform_view.hpp>

struct basic_shapes_policy
{
  typedef boost::mpl::set<
    svgpp::tag::element::rect,
    svgpp::tag::element::line,
    svgpp::tag::element::circle,
    svgpp::tag::element::ellipse
  > convert_to_path;

  typedef boost::mpl::set<
  > collect_attributes;

  static const bool convert_only_rounded_rect_to_path = true;
  static const bool viewport_as_transform = true;
  static const bool calculate_viewport = true;
  static const bool polyline_as_path = true;
};

class Canvas: boost::noncopyable
{
public:
  void on_enter_element(svgpp::tag::element::any const &)
  {
  }

  void on_exit_element()
  {
  }

  void set_transform_matrix(const boost::array<double, 6> & matrix)
  {
  }

  void path_move_to(double x, double y, svgpp::tag::absolute_coordinate const &)
  { 
  }

  void path_line_to(double x, double y, svgpp::tag::absolute_coordinate const &)
  { 
  }

  void path_cubic_bezier_to(
    double x1, double y1, 
    double x2, double y2, 
    double x, double y, 
    svgpp::tag::absolute_coordinate const &)
  { 
  }

  void path_close_subpath()
  {
  }

  void path_exit()
  {
  }

  void set_rect(double, double, double, double) {}
};

int main()
{
  char text[] = "<svg/>";
  rapidxml::xml_document<> doc;    // character type defaults to char
  doc.parse<0>(text);  
  if (rapidxml::xml_node<> * svg_element = doc.first_node("svg"))
  {
    Canvas canvas;
    svgpp::document_traversal<
      svgpp::processed_elements<boost::mpl::set<
        svgpp::tag::element::svg,
        svgpp::tag::element::g,
        svgpp::tag::element::path,
        svgpp::tag::element::rect,
        svgpp::tag::element::line,
        svgpp::tag::element::circle,
        svgpp::tag::element::ellipse,
        svgpp::tag::element::polyline
      > >,
      svgpp::processed_attributes<
        boost::mpl::fold<
          boost::mpl::protect<
            boost::mpl::joint_view<
              boost::mpl::transform_view<
                svgpp::rect_shape_attributes, boost::mpl::pair<svgpp::tag::element::rect, boost::mpl::_1> >,
              boost::mpl::transform_view<
                  svgpp::traits::viewport_attributes, boost::mpl::pair<svgpp::tag::element::svg, boost::mpl::_1> >
            >
          >,
          boost::mpl::set<
            svgpp::tag::attribute::d,
            svgpp::tag::attribute::transform,
            svgpp::tag::attribute::points,
            svgpp::tag::attribute::x1,
            svgpp::tag::attribute::y1,
            svgpp::tag::attribute::x2,
            svgpp::tag::attribute::y2,
            svgpp::tag::attribute::cx,
            svgpp::tag::attribute::cy,
            svgpp::tag::attribute::r,
            svgpp::tag::attribute::rx,
            svgpp::tag::attribute::ry
          >,
          boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
        >::type
      >,
      svgpp::basic_shapes_policy<basic_shapes_policy>,
      svgpp::path_policy<svgpp::path_policies_minimal>
    >::load_document(svg_element, canvas);
  }
  return 0;
}