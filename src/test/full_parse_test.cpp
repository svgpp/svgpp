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

struct LoadValuePolicy
{
  template<class AttributeTag, class T1>
  static void set(Context & context, AttributeTag tag, T1 const & value)
  {
    CheckAttributeTag<AttributeTag>();
    UseValue(value);
  }

  template<class AttributeTag, class T1>
  static void set(Context & context, AttributeTag tag, T1 const & value, tag::source::attribute)
  {
    CheckAttributeTag<AttributeTag>();
    UseValue(value);
  }

  template<class AttributeTag, class T1>
  static void set(Context & context, AttributeTag tag, T1 const & value, tag::source::css)
  {
    CheckAttributeTag<AttributeTag>();
    UseValue(value);
  }

  template<class AttributeTag, class T1, class T2>
  static void set(Context & context, AttributeTag tag, T1 const & value1, T2 const & value2)
  {
    CheckAttributeTag<AttributeTag>();
    UseValue(value1);
    UseValue(value2);
  }

  template<class AttributeTag, class T1, class T2, class T3>
  static void set(Context & context, AttributeTag tag, T1 const & value1, T2 const & value2, T3 const & value3)
  {
    CheckAttributeTag<AttributeTag>();
    UseValue(value1);
    UseValue(value2);
    UseValue(value3);
  }

  template<class AttributeTag, class T1, class T2, class T3, class T4>
  static void set(Context & context, AttributeTag tag, T1 const & value1, T2 const & value2, T3 const & value3, 
    T4 const & value4)
  {
    CheckAttributeTag<AttributeTag>();
    UseValue(value1);
    UseValue(value2);
    UseValue(value3);
    UseValue(value4);
  }

  template<class AttributeTag, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  static void set(Context & context, AttributeTag tag, T1 const & value1, T2 const & value2, T3 const & value3, 
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

struct LoadTextPolicy
{
  template<class T>
  static void set_text(Context & context, T const & value)
  {
    UseValue(value);
  }
};

struct LoadTransformPolicy
{
  typedef Context context_type;

  static void set_transform_matrix(Context & context, const boost::array<double, 6> & matrix)
  {
    for(int i=0; i<6; ++i)
      UseValue(matrix[i]);
  }
};

struct LoadPathPolicy
{
  typedef Context context_type;

  static void path_move_to(Context & context, double x, double y, tag::absolute_coordinate)
  { 
    UseValue(x);
    UseValue(y);
  }

  static void path_line_to(Context & context, double x, double y, tag::absolute_coordinate)
  { 
    UseValue(x);
    UseValue(y);
  }

  static void path_cubic_bezier_to(Context & context, double x1, double y1, 
                                        double x2, double y2, 
                                        double x, double y, 
                                        tag::absolute_coordinate)
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
                                        tag::absolute_coordinate)
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
                                        tag::absolute_coordinate)
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

namespace svgpp { namespace policy { namespace load_value 
{
  template<>
  struct default_policy<Context>: LoadValuePolicy
  {};
}}}

int main()
{
  rapidxml_ns::xml_document<char> doc;
  rapidxml_ns::xml_node<char> const * svg_element = doc.first_node();

  Context context;
  document_traversal<
    viewport_policy<policy::viewport::raw>,
    load_text_policy<LoadTextPolicy>,
    load_transform_policy<LoadTransformPolicy>,
    load_path_policy<LoadPathPolicy>,
    ignored_elements<boost::mpl::set1<tag::element::filter> >,
    ignored_attributes<boost::mpl::set<
      tag::attribute::cursor,
      tag::attribute::font_size,
      tag::attribute::glyph_name,
      tag::attribute::u1,
      tag::attribute::u2,
      tag::attribute::g1,
      tag::attribute::g2,
      boost::mpl::pair<tag::element::animate, tag::attribute::values>,
      boost::mpl::pair<tag::element::animateColor, tag::attribute::values>,
      boost::mpl::pair<tag::element::animateMotion, tag::attribute::values>,
      boost::mpl::pair<tag::element::animateTransform, tag::attribute::values>,
      tag::attribute::keyTimes,
      tag::attribute::keySplines,
      tag::attribute::unicode_range,
      tag::attribute::panose_1,
      tag::attribute::widths
    >::type>
  >::load_document(svg_element, context);

  return 0;
}