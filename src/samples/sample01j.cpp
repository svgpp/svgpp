#define BOOST_PARAMETER_MAX_ARITY 11
// Following defines move parts of SVG++ code to svgpp_parser_impl.cpp file
// reducing compiler memory requirements
#define SVGPP_USE_EXTERNAL_PATH_DATA_PARSER
#define SVGPP_USE_EXTERNAL_TRANSFORM_PARSER
#define SVGPP_USE_EXTERNAL_PRESERVE_ASPECT_RATIO_PARSER
#define SVGPP_USE_EXTERNAL_PAINT_PARSER
#define SVGPP_USE_EXTERNAL_MISC_PARSER

#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>
#include <rapidxml_ns/rapidxml_ns_utils.hpp>
#include <svgpp/svgpp.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/variant.hpp>
#include <boost/mpl/set/set30.hpp>

#include "sample01j.hpp"

using namespace svgpp;

typedef rapidxml_ns::xml_node<> const * xml_element_t;

typedef boost::variant<tag::value::none, tag::value::currentColor, color_t> SolidPaint;
struct IRIPaint
{
  IRIPaint(std::string const & fragment, boost::optional<SolidPaint> const & fallback = boost::optional<SolidPaint>())
    : fragment_(fragment)
    , fallback_(fallback)
  {}

  std::string fragment_;
  boost::optional<SolidPaint> fallback_;
};
typedef boost::variant<SolidPaint, IRIPaint> Paint;

class StylableContext
{
public:
  StylableContext()
    : stroke_width_(1)
  {}

  void set(tag::attribute::stroke_width, double val)
    { stroke_width_ = val; }

  void set(tag::attribute::stroke, tag::value::none)
    { stroke_ = tag::value::none(); }

  void set(tag::attribute::stroke, tag::value::currentColor)
    { stroke_ = tag::value::currentColor(); }

  void set(tag::attribute::stroke, color_t color, tag::skip_icc_color = tag::skip_icc_color())
    { stroke_ = color; }

  template<class IRI>
  void set(tag::attribute::stroke tag, IRI const & iri)
    { throw std::runtime_error("Non-local references aren't supported"); }

  template<class IRI>
  void set(tag::attribute::stroke tag, tag::iri_fragment, IRI const & fragment)
    { stroke_ = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment))); }

  template<class IRI>
  void set(tag::attribute::stroke tag, IRI const &, tag::value::none val)
    { set(tag, val); }

  template<class IRI>
  void set(tag::attribute::stroke tag, tag::iri_fragment, IRI const & fragment, tag::value::none val)
    { stroke_ = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); }

  template<class IRI>
  void set(tag::attribute::stroke tag, IRI const &, tag::value::currentColor val)
    { set(tag, val); }

  template<class IRI>
  void set(tag::attribute::stroke tag, tag::iri_fragment, IRI const & fragment, tag::value::currentColor val)
    { stroke_ = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); }

  template<class IRI>
  void set(tag::attribute::stroke tag, IRI const &, color_t val, tag::skip_icc_color = tag::skip_icc_color())
    { set(tag, val); }

  template<class IRI>
  void set(tag::attribute::stroke tag, tag::iri_fragment, IRI const & fragment, color_t val, tag::skip_icc_color = tag::skip_icc_color())
    { stroke_ = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); }

protected:
  Paint stroke_;
  double stroke_width_;
};

class BaseContext: public StylableContext
{
public:
  BaseContext(double resolutionDPI)
  {
    length_factory_.set_absolute_units_coefficient(resolutionDPI, tag::length_units::in());
  }

  // Called by Context Factory
  void on_exit_element()
  {}

  // Transform Events Policy
  void transform_matrix(const boost::array<double, 6> & matrix)
  {}

  // Viewport Events Policy
  void set_viewport(double viewport_x, double viewport_y, double viewport_width, double viewport_height)
  {
    length_factory_.set_viewport_size(viewport_width, viewport_height);
  }

  void set_viewbox_size(double viewbox_width, double viewbox_height)
  {
    length_factory_.set_viewport_size(viewbox_width, viewbox_height);
  }

  void disable_rendering()
  {}

  // Length Policy interface
  typedef factory::length::unitless<> length_factory_type;

  length_factory_type const & length_factory() const
  { return length_factory_; }

private:
  length_factory_type length_factory_;
};

class ShapeContext: public BaseContext
{
public:
  ShapeContext(BaseContext const & parent)
    : BaseContext(parent)
  {}

  // Path Events Policy methods
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

  // Marker Events Policy method
  void marker(marker_vertex v, double x, double y, double directionality, unsigned marker_index)
  {
    if (marker_index >= markers_.size())
      markers_.resize(marker_index + 1);
    MarkerPos & m = markers_[marker_index];
    m.v = v;
    m.x = x;
    m.y = y;
    m.directionality = directionality;
  }

private:
  struct MarkerPos
  {
    marker_vertex v;
    double x, y, directionality;
  };

  typedef std::vector<MarkerPos> Markers; 
  Markers markers_;
};

class UseContext: public BaseContext
{
public:
  UseContext(BaseContext const & parent)
    : BaseContext(parent)
  {}

  boost::optional<double> const & width() const { return width_; }
  boost::optional<double> const & height() const { return height_; }

  using BaseContext::set;

  template<class IRI>
  void set(tag::attribute::xlink::href, tag::iri_fragment, IRI const & fragment)
  { fragment_id_.assign(boost::begin(fragment), boost::end(fragment)); }

  template<class IRI>
  void set(tag::attribute::xlink::href, IRI const & fragment)
  { std::cerr << "External references aren't supported\n"; }

  void set(tag::attribute::x, double val)
  { x_ = val; }

  void set(tag::attribute::y, double val)
  { y_ = val; }

  void set(tag::attribute::width, double val)
  { width_ = val; }

  void set(tag::attribute::height, double val)
  { height_ = val; }

  void on_exit_element();

private:
  std::string fragment_id_;
  double x_, y_;
  boost::optional<double> width_, height_;
};

class ReferencedSymbolOrSvgContext: 
  public BaseContext
{
public:
  ReferencedSymbolOrSvgContext(UseContext & referencing)
    : BaseContext(referencing)
    , referencing_(referencing)
  {
  }

  // Viewport Events Policy
  void get_reference_viewport_size(double & width, double & height)
  {
    if (referencing_.width())
      width = *referencing_.width();
    if (referencing_.height())
      height = *referencing_.height();
  }

private:
  UseContext & referencing_;
};

class TextContext: public BaseContext
{
public:
  TextContext(BaseContext const & parent)
    : BaseContext(parent)
    , x_(0)
    , y_(0)
  {}

  template<class Range>
  void set_text(Range const & text)
  {
    text_content_.append(boost::begin(text), boost::end(text));
  }

  using StylableContext::set;

  template<class Range>
  void set(tag::attribute::x, Range const & range)
  { 
    for(typename boost::range_iterator<Range>::type it = boost::begin(range), end = boost::end(range);
      it != end; ++it)
      std::cout << *it;
  }

  template<class Range>
  void set(tag::attribute::y, Range const & range)
  { 
    for(typename boost::range_iterator<Range>::type it = boost::begin(range), end = boost::end(range);
      it != end; ++it)
      std::cout << *it;
  }

private:
  std::string text_content_;
  double x_, y_;
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

template<>
struct ChildContextFactories::apply<BaseContext, tag::element::use_>
{
  typedef factory::context::on_stack<UseContext> type;
};

// Elements referenced by 'use' element
template<>
struct ChildContextFactories::apply<UseContext, tag::element::svg, void>
{
  typedef factory::context::on_stack<ReferencedSymbolOrSvgContext> type;
};

template<>
struct ChildContextFactories::apply<UseContext, tag::element::symbol, void>
{
  typedef factory::context::on_stack<ReferencedSymbolOrSvgContext> type;
};

template<class ElementTag>
struct ChildContextFactories::apply<UseContext, ElementTag, void>: ChildContextFactories::apply<BaseContext, ElementTag>
{};

template<class ElementTag>
struct ChildContextFactories::apply<ReferencedSymbolOrSvgContext, ElementTag, void>: 
  ChildContextFactories::apply<BaseContext, ElementTag>
{};

template<>
struct ChildContextFactories::apply<BaseContext, tag::element::text>
{
  typedef factory::context::on_stack<TextContext> type;
};

struct AttributeTraversal: policy::attribute_traversal::default_policy
{
  typedef boost::mpl::if_<
    // If element is 'svg' or 'symbol'...
    boost::mpl::has_key<
      boost::mpl::set<
        tag::element::svg,
        tag::element::symbol
      >,
      boost::mpl::_1
    >,
    boost::mpl::vector<
      // ... load viewport-related attributes first ...
      tag::attribute::x, 
      tag::attribute::y, 
      tag::attribute::width, 
      tag::attribute::height, 
      tag::attribute::viewBox, 
      tag::attribute::preserveAspectRatio,
      // ... notify library, that all viewport attributes that are present was loaded.
      // It will result in call to BaseContext::set_viewport and BaseContext::set_viewbox_size
      notify_context<tag::event::after_viewport_attributes>
    >::type,
    boost::mpl::empty_sequence
  > get_priority_attributes_by_element;
};


struct processed_elements_t
  : boost::mpl::set<
    // SVG Structural Elements
    tag::element::svg,
    tag::element::g,
    tag::element::use_,
    // SVG Shape Elements
    tag::element::circle,
    tag::element::ellipse,
    tag::element::line,
    tag::element::path,
    tag::element::polygon,
    tag::element::polyline,
    tag::element::rect,
    // Text Element
    tag::element::text
    >
{};

// Joining some sequences from traits namespace with chosen attributes
struct processed_attributes_t
  : boost::mpl::set30<
      svgpp::tag::attribute::transform,
      svgpp::tag::attribute::stroke,
      svgpp::tag::attribute::stroke_width,
      boost::mpl::pair<svgpp::tag::element::use_, svgpp::tag::attribute::xlink::href>,
      // traits::shapes_attributes_by_element, 
      boost::mpl::pair<tag::element::path, tag::attribute::d>,
      boost::mpl::pair<tag::element::rect, tag::attribute::x>,
      boost::mpl::pair<tag::element::rect, tag::attribute::y>,
      boost::mpl::pair<tag::element::rect, tag::attribute::width>,
      boost::mpl::pair<tag::element::rect, tag::attribute::height>,
      boost::mpl::pair<tag::element::rect, tag::attribute::rx>,
      boost::mpl::pair<tag::element::rect, tag::attribute::ry>,
      boost::mpl::pair<tag::element::circle, tag::attribute::cx>,
      boost::mpl::pair<tag::element::circle, tag::attribute::cy>,
      boost::mpl::pair<tag::element::circle, tag::attribute::r>,
      boost::mpl::pair<tag::element::ellipse, tag::attribute::cx>,
      boost::mpl::pair<tag::element::ellipse, tag::attribute::cy>,
      boost::mpl::pair<tag::element::ellipse, tag::attribute::rx>,
      boost::mpl::pair<tag::element::ellipse, tag::attribute::ry>,
      boost::mpl::pair<tag::element::line, tag::attribute::x1>,
      boost::mpl::pair<tag::element::line, tag::attribute::y1>,
      boost::mpl::pair<tag::element::line, tag::attribute::x2>,
      boost::mpl::pair<tag::element::line, tag::attribute::y2>,
      boost::mpl::pair<tag::element::polyline, tag::attribute::points>,
      boost::mpl::pair<tag::element::polygon, tag::attribute::points>,
      // traits::viewport_attributes
      tag::attribute::x,
      tag::attribute::y,
      tag::attribute::width,
      tag::attribute::height,
      tag::attribute::viewBox,
      tag::attribute::preserveAspectRatio
    >
{};

typedef
  document_traversal<
    processed_elements<processed_elements_t>,
    processed_attributes<processed_attributes_t>,
    viewport_policy<policy::viewport::as_transform>,
    context_factories<ChildContextFactories>,
    markers_policy<policy::markers::calculate_always>,
    color_factory<ColorFactory>,
    length_policy<policy::length::forward_to_method<BaseContext> >,
    attribute_traversal_policy<AttributeTraversal>,
    transform_events_policy<policy::transform_events::forward_to_method<BaseContext> > // Same as default, but less instantiations
  > document_traversal_t;

void loadSvg(xml_element_t xml_root_element)
{
  static const double ResolutionDPI = 90;
  BaseContext context(ResolutionDPI);
  document_traversal_t::load_document(xml_root_element, context);
}

xml_element_t FindCurrentDocumentElementById(std::string const &) { return NULL; }

struct processed_elements_with_symbol_t
  : boost::mpl::insert<processed_elements_t::type, tag::element::symbol>::type
{};

void UseContext::on_exit_element()
{
  if (xml_element_t element = FindCurrentDocumentElementById(fragment_id_))
  {
    // TODO: Check for cyclic references
    // TODO: Apply translate transform (x_, y_)
    document_traversal_t::load_referenced_element<
      referencing_element<tag::element::use_>,
      expected_elements<traits::reusable_elements>,
      processed_elements<processed_elements_with_symbol_t>
    >::load(element, *this);
  }
  else
    std::cerr << "Element referenced by 'use' not found\n";
}

int main(int argc, char * argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " <svg file name>\n";
    return 1;
  }

  try
  {
    rapidxml_ns::file<> xml_file(argv[1]);

    rapidxml_ns::xml_document<> doc;
    doc.parse<rapidxml_ns::parse_no_string_terminators>(xml_file.data());  
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
