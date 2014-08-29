#pragma once

struct test_path_context
{
  template<class AbsoluteTag>
  void path_move_to(double x, double y, AbsoluteTag)
  { 
    log_ << (AbsoluteTag::is_absolute ? "M" : "m") << x << "," << y;
  }

  template<class AbsoluteTag>
  void path_line_to(double x, double y, AbsoluteTag)
  { 
    log_ << (AbsoluteTag::is_absolute ? "L" : "l") << x << "," << y;
  }

  template<class AbsoluteTag>
  void path_line_to_ortho(double coord, bool horizontal, AbsoluteTag)
  {
    log_ << (AbsoluteTag::is_absolute ? (horizontal ? "H" : "V") : (horizontal ? "h" : "v")) << coord;
  }

  template<class AbsoluteTag>
  void path_cubic_bezier_to(double x1, double y1, 
    double x2, double y2, double x, double y, 
    AbsoluteTag)
  {
    log_ << (AbsoluteTag::is_absolute ? "C" : "c") << x1 << "," << y1 << "," << x2 << "," << y2 << "," << x << "," << y;
  }

  template<class AbsoluteTag>
  void path_cubic_bezier_to(
    double x2, double y2, double x, double y, 
    AbsoluteTag)
  {
    log_ << (AbsoluteTag::is_absolute ? "S" : "s") << x2 << "," << y2 << "," << x << "," << y;
  }

  template<class AbsoluteTag>
  void path_quadratic_bezier_to(
    double x1, double y1, double x, double y, 
    AbsoluteTag)
  {
    log_ << (AbsoluteTag::is_absolute ? "Q" : "q") << x1 << "," << y1 << "," << x << "," << y;
  }

  template<class AbsoluteTag>
  void path_quadratic_bezier_to(double x, double y, AbsoluteTag)
  {
    log_ << (AbsoluteTag::is_absolute ? "T" : "t") << x << "," << y;
  }

  template<class AbsoluteTag>
  void path_elliptical_arc_to(
    double rx, double ry, double x_axis_rotation,
    bool large_arc_flag, bool sweep_flag, 
    double x, double y,
    AbsoluteTag)
  { 
    log_ << (AbsoluteTag::is_absolute ? "A" : "a") << rx << "," << ry << "," << x_axis_rotation << "," 
      << large_arc_flag << "," << sweep_flag << "," << x << "," << y;
  }

  void path_close_subpath()
  { 
    log_ << "Z"; 
  }

  void path_exit()
  {}

  std::string str() const { return log_.str(); }

private:
  std::ostringstream log_;
};