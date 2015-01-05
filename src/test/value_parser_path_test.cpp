#include <svgpp/parser/path_data.hpp>

// Only compilation is checked

using namespace svgpp;

namespace
{
  struct PathContext1
  {
  public:
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

  struct PathContext2
  {
    void path_move_to(double x, double y, tag::coordinate::absolute)
    {}

    void path_line_to(double x, double y, tag::coordinate::absolute)
    {}

    void path_line_to_ortho(double coord, bool horizontal, tag::coordinate::absolute)
    {}

    void path_cubic_bezier_to(double x1, double y1, 
                                          double x2, double y2, 
                                          double x, double y, 
                                          tag::coordinate::absolute)
    {}

    void path_cubic_bezier_to(
                                          double x2, double y2, 
                                          double x, double y, 
                                          tag::coordinate::absolute)
    {}

    void path_quadratic_bezier_to(
                                          double x1, double y1, 
                                          double x, double y, 
                                          tag::coordinate::absolute)
    {}

    void path_quadratic_bezier_to(
                                          double x, double y, 
                                          tag::coordinate::absolute)
    {}

    void path_elliptical_arc_to(
                                          double rx, double ry, double x_axis_rotation,
                                          bool large_arc_flag, bool sweep_flag, 
                                          double x, double y,
                                          tag::coordinate::absolute)
    {}

    void path_move_to(double x, double y, tag::coordinate::relative)
    {}

    void path_line_to(double x, double y, tag::coordinate::relative)
    {}

    void path_line_to_ortho(double coord, bool horizontal, tag::coordinate::relative)
    {}

    void path_cubic_bezier_to(double x1, double y1, 
                                          double x2, double y2, 
                                          double x, double y, 
                                          tag::coordinate::relative)
    {}

    void path_cubic_bezier_to(
                                          double x2, double y2, 
                                          double x, double y, 
                                          tag::coordinate::relative)
    {}

    void path_quadratic_bezier_to(
                                          double x1, double y1, 
                                          double x, double y, 
                                          tag::coordinate::relative)
    {}

    void path_quadratic_bezier_to(
                                          double x, double y, 
                                          tag::coordinate::relative)
    {}

    void path_elliptical_arc_to(
                                          double rx, double ry, double x_axis_rotation,
                                          bool large_arc_flag, bool sweep_flag, 
                                          double x, double y,
                                          tag::coordinate::relative)
    {}

    void path_close_subpath()
    {}

    void path_exit()
    {}
  };
}

void check_path()
{
  std::string testStr;

  {
    PathContext1 ctx;
    value_parser<tag::type::path_data>::parse(
      tag::attribute::d(), ctx, testStr, tag::source::attribute());
  }

  {
    PathContext2 ctx;
    value_parser<
      tag::type::path_data,
      path_policy<policy::path::raw>
    >::parse(
      tag::attribute::d(), ctx, testStr, tag::source::attribute());
  }
}