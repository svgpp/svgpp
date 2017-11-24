#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>
#include <svgpp/document_traversal.hpp>
#include <boost/mpl/set.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

class Gradient;

class GradientStop
{
public:
  GradientStop(Gradient & parent)
    : parent_(parent)
  {
  }

  void on_exit_element();

  void set(svgpp::tag::attribute::offset, double val)
  {
    stop_ = val;
  }

private:
  Gradient & parent_;
  double stop_;
};

class Gradient
{
public:
  enum SpreadMethod { spreadPad, spreadReflect, spreadRepeat };

  Gradient()
    : useObjectBoundingBox_(true)
    , spreadMethod_(spreadPad)
  {
  }

  void add_stop(double stop)
  {
    stops_.push_back(stop);
  }

  void on_exit_element()
  {
  }

  void transform_matrix(const boost::array<double, 6> & matrix)
  {
    matrix_ = matrix;
  }

  template<class StringRange>
  void set(svgpp::tag::attribute::id, StringRange const & str)
  {
    id_.assign(boost::begin(str), boost::end(str));
  }

  void set(svgpp::tag::attribute::gradientUnits, svgpp::tag::value::userSpaceOnUse)
  {
    useObjectBoundingBox_ = false;
  }

  void set(svgpp::tag::attribute::gradientUnits, svgpp::tag::value::objectBoundingBox)
  {
    useObjectBoundingBox_ = true;
  }

  void set(svgpp::tag::attribute::spreadMethod, svgpp::tag::value::pad)
  {
    spreadMethod_ = spreadPad;
  }

  void set(svgpp::tag::attribute::spreadMethod, svgpp::tag::value::reflect)
  {
    spreadMethod_ = spreadReflect;
  }

  void set(svgpp::tag::attribute::spreadMethod, svgpp::tag::value::repeat)
  {
    spreadMethod_ = spreadRepeat;
  }

protected:
  std::string id_;
  boost::array<double, 6> matrix_;
  bool useObjectBoundingBox_;
  SpreadMethod spreadMethod_;
  std::vector<double> stops_;
};

class LinearGradient: public Gradient
{
public:
  LinearGradient()
  {
  }

  using Gradient::set;

  void set(svgpp::tag::attribute::x1, double val)
  {
    x1_ = val;
  }

  void set(svgpp::tag::attribute::y1, double val)
  {
    y1_ = val;
  }

  void set(svgpp::tag::attribute::x2, double val)
  {
    x2_ = val;
  }

  void set(svgpp::tag::attribute::y2, double val)
  {
    y2_ = val;
  }

private:
  double x1_, y1_, x2_, y2_;
};

class RadialGradient: public Gradient
{
public:
  RadialGradient()
  {
  }

  using Gradient::set;

  void set(svgpp::tag::attribute::cx, double val)
  {
    cx_ = val;
  }

  void set(svgpp::tag::attribute::cy, double val)
  {
    cy_ = val;
  }

  void set(svgpp::tag::attribute::r, double val)
  {
    r_ = val;
  }

  void set(svgpp::tag::attribute::fx, double val)
  {
    fx_ = val;
  }

  void set(svgpp::tag::attribute::fy, double val)
  {
    fy_ = val;
  }

private:
  double cx_, cy_, fx_, fy_, r_;
};

void GradientStop::on_exit_element()
{
  parent_.add_stop(stop_);
}

class Canvas
{
public:
  void on_enter_element(svgpp::tag::element::svg const &)
  {
  }

  void on_exit_element()
  {
  }

  boost::shared_ptr<LinearGradient> get_child_context(svgpp::tag::element::linearGradient const &)
  {
    return boost::make_shared<LinearGradient>();
  }

  boost::shared_ptr<RadialGradient> get_child_context(svgpp::tag::element::radialGradient const &)
  {
    return boost::make_shared<RadialGradient>();
  }
};

struct ContextFactories
{
  template<class ParentContext, class ElementTag>
  struct apply
  {
    typedef svgpp::factory::context::same<ParentContext, ElementTag> type;
  };
};

template<class ParentContext>
struct ContextFactories::apply<ParentContext, svgpp::tag::element::linearGradient>
{
  typedef svgpp::factory::context::get_ptr_from_parent<svgpp::tag::element::linearGradient, boost::shared_ptr<LinearGradient> > type;
};

template<class ParentContext>
struct ContextFactories::apply<ParentContext, svgpp::tag::element::radialGradient>
{
  typedef svgpp::factory::context::get_ptr_from_parent<svgpp::tag::element::radialGradient, boost::shared_ptr<RadialGradient> > type;
};

template<class ParentContext>
struct ContextFactories::apply<ParentContext, svgpp::tag::element::stop>
{
  typedef svgpp::factory::context::on_stack<GradientStop> type;
};

int main()
{
#define TEXT(x) #x
  char text[] = 
    TEXT(<linearGradient id="MyGradient" xmlns="http://www.w3.org/2000/svg">)
    TEXT( <stop offset="5%" stop-color="#F60"/>)
    TEXT( <stop offset="95%" stop-color="#FF6"/>)
    TEXT(</linearGradient>);

  rapidxml_ns::xml_document<> doc;    // character type defaults to char
  doc.parse<0>(text);  
  if (rapidxml_ns::xml_node<> * gradient_element = doc.first_node())
  {
    Canvas canvas;
    svgpp::document_traversal<
      svgpp::context_factories<ContextFactories>,
      svgpp::processed_elements<
        boost::mpl::set<
          svgpp::tag::element::linearGradient,
          svgpp::tag::element::radialGradient,
          svgpp::tag::element::stop
        >::type
      >,
      svgpp::processed_attributes<
        boost::mpl::set<
          boost::mpl::pair<svgpp::tag::element::linearGradient, svgpp::tag::attribute::id>,
          boost::mpl::pair<svgpp::tag::element::linearGradient, svgpp::tag::attribute::x1>,
          boost::mpl::pair<svgpp::tag::element::linearGradient, svgpp::tag::attribute::y1>,
          boost::mpl::pair<svgpp::tag::element::linearGradient, svgpp::tag::attribute::x2>,
          boost::mpl::pair<svgpp::tag::element::linearGradient, svgpp::tag::attribute::y2>,
          boost::mpl::pair<svgpp::tag::element::radialGradient, svgpp::tag::attribute::id>,
          boost::mpl::pair<svgpp::tag::element::radialGradient, svgpp::tag::attribute::fx>,
          boost::mpl::pair<svgpp::tag::element::radialGradient, svgpp::tag::attribute::fy>,
          boost::mpl::pair<svgpp::tag::element::radialGradient, svgpp::tag::attribute::cx>,
          boost::mpl::pair<svgpp::tag::element::radialGradient, svgpp::tag::attribute::cy>,
          boost::mpl::pair<svgpp::tag::element::radialGradient, svgpp::tag::attribute::r>,
          svgpp::tag::attribute::gradientUnits,
          svgpp::tag::attribute::gradientTransform,
          svgpp::tag::attribute::spreadMethod,
          boost::mpl::pair<svgpp::tag::element::stop, svgpp::tag::attribute::offset>
        >::type
      >
    >::load_referenced_element<
      svgpp::expected_elements<svgpp::traits::gradient_elements>
    >::load(gradient_element, canvas);
  }
  return 0;
}