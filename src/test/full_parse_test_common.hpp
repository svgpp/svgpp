// Following defines move parts of SVG++ code to full_parse_main.cpp file
// reducing compiler memory requirements
#define SVGPP_USE_EXTERNAL_PATH_DATA_PARSER
#define SVGPP_USE_EXTERNAL_TRANSFORM_PARSER
#define SVGPP_USE_EXTERNAL_PRESERVE_ASPECT_RATIO_PARSER
#define SVGPP_USE_EXTERNAL_PAINT_PARSER
#define SVGPP_USE_EXTERNAL_MISC_PARSER
#define SVGPP_USE_EXTERNAL_COLOR_PARSER
#define SVGPP_USE_EXTERNAL_LENGTH_PARSER

#include <svgpp/svgpp.hpp>
#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>

using namespace svgpp;

struct Context 
{
  void on_enter_element(tag::element::any) {}
  void on_exit_element() {}
};

template<class AttributeTag>
inline void CheckAttributeTag()
{
  detail::attribute_id id = AttributeTag::attribute_id;
}

// Inhibit optimizing off value
template<class T>
inline void UseValue(T const & value)
{
  volatile static T val = value;
}

inline void CheckCall()
{
  volatile static int i = 0;
  ++i;
}

struct ValueEventsPolicy
{
  template<class AttributeTag, class T1>
  static void set(Context & context, AttributeTag, tag::source::any const &, T1 const & value)
  {
    CheckAttributeTag<AttributeTag>();
    UseValue(value);
  }

  template<class AttributeTag, class T1, class T2>
  static void set(Context & context, AttributeTag, tag::source::any const &, T1 const & value1, T2 const & value2)
  {
    CheckAttributeTag<AttributeTag>();
    UseValue(value1);
    UseValue(value2);
  }

  template<class AttributeTag, class T1, class T2, class T3>
  static void set(Context & context, AttributeTag, tag::source::any const &, T1 const & value1, T2 const & value2, T3 const & value3)
  {
    CheckAttributeTag<AttributeTag>();
    UseValue(value1);
    UseValue(value2);
    UseValue(value3);
  }

  template<class AttributeTag, class T1, class T2, class T3, class T4>
  static void set(Context & context, AttributeTag, tag::source::any const &, T1 const & value1, T2 const & value2, T3 const & value3, 
    T4 const & value4)
  {
    CheckAttributeTag<AttributeTag>();
    UseValue(value1);
    UseValue(value2);
    UseValue(value3);
    UseValue(value4);
  }

  template<class AttributeTag, class T1, class T2, class T3, class T4, class T5>
  static void set(Context & context, AttributeTag, tag::source::any const &, T1 const & value1, T2 const & value2, T3 const & value3, 
    T4 const & value4, T5 const & value5)
  {
    CheckAttributeTag<AttributeTag>();
    UseValue(value1);
    UseValue(value2);
    UseValue(value3);
    UseValue(value4);
    UseValue(value5);
  }

  template<class AttributeTag, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  static void set(Context & context, AttributeTag, tag::source::any const &, T1 const & value1, T2 const & value2, T3 const & value3, 
    T4 const & value4, T5 const & value5, T6 const & value6, T7 const & value7, T8 const & value8)
  {
    CheckAttributeTag<AttributeTag>();
    UseValue(value1);
    UseValue(value2);
    UseValue(value3);
    UseValue(value4);
    UseValue(value5);
    UseValue(value6);
    UseValue(value7);
    UseValue(value8);
  }
};

struct TextEventsPolicy
{
  template<class T>
  static void set_text(Context & context, T const & value)
  {
    UseValue(value);
  }
};

struct TransformEventsPolicy
{
  typedef Context context_type;

  static void transform_matrix(Context & context, const boost::array<double, 6> & matrix)
  {
    for(int i=0; i<6; ++i)
      UseValue(matrix[i]);
  }
};

struct PathEventsPolicy
{
  typedef Context context_type;

  static void path_move_to(Context & context, double x, double y, tag::coordinate::absolute)
  { 
    UseValue(x);
    UseValue(y);
  }

  static void path_line_to(Context & context, double x, double y, tag::coordinate::absolute)
  { 
    UseValue(x);
    UseValue(y);
  }

  static void path_cubic_bezier_to(Context & context, double x1, double y1, 
                                        double x2, double y2, 
                                        double x, double y, 
                                        tag::coordinate::absolute)
  { 
    UseValue(x1);
    UseValue(y1);
    UseValue(x2);
    UseValue(y2);
    UseValue(x);
    UseValue(y);
  }

  static void path_quadratic_bezier_to(Context & context, 
                                        double x1, double y1, 
                                        double x, double y, 
                                        tag::coordinate::absolute)
  { 
    UseValue(x);
    UseValue(y);
    UseValue(x1);
    UseValue(y1);
  }

  static void path_elliptical_arc_to(Context & context, 
                                        double rx, double ry, double x_axis_rotation,
                                        bool large_arc_flag, bool sweep_flag, 
                                        double x, double y,
                                        tag::coordinate::absolute)
  { 
    UseValue(x);
    UseValue(y);
    UseValue(rx);
    UseValue(ry);
    UseValue(x_axis_rotation);
    UseValue(large_arc_flag);
    UseValue(sweep_flag);
  }

  static void path_close_subpath(Context & context)
  {
    CheckCall();
  }

  static void path_exit(Context & context)
  { 
    CheckCall();
  }
};

namespace svgpp { namespace policy { namespace value_events 
{
  template<>
  struct default_policy<Context>: ValueEventsPolicy
  {};
}}}

void parse(rapidxml_ns::xml_node<char> const * svg_element);