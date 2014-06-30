#include "gradient.hpp"

#include <svgpp/xml/rapidxml_ns.hpp>
#include <svgpp/document_traversal.hpp>
#include <boost/mpl/set.hpp>

namespace
{

class GradientBaseContext;

typedef boost::optional<Gradient> GradientContext;

class GradientStopContext
{
public:
  GradientStopContext(GradientBaseContext & parent)
    : parent_(parent)
    , opacity_(1)
  {}

  void on_exit_element();

  void set(svgpp::tag::attribute::offset, double val)
  { data_.offset_ = std::min(1.0, std::max(0.0, val)); }

  void set(svgpp::tag::attribute::stop_color, svgpp::tag::value::inherit)
  {} // TODO

  void set(svgpp::tag::attribute::stop_color, svgpp::tag::value::currentColor)
  {} // TODO

  void set(svgpp::tag::attribute::stop_color, agg::rgba8 color, svgpp::tag::skip_icc_color = svgpp::tag::skip_icc_color())
  { data_.color_ = color; }

  void set(svgpp::tag::attribute::stop_opacity, double val)
  { opacity_ = std::min(1.0, std::max(0.0, val)); }

  void set(svgpp::tag::attribute::stop_opacity, svgpp::tag::value::inherit)
  {} // TODO

private:
  GradientBaseContext & parent_;
  GradientStop data_;
  double opacity_;
};

class GradientBaseContext
{
public:
  GradientBaseContext(GradientBase & data)
    : data_(data)
  {}

  void add_stop(GradientStop stop)
  {
    if (!data_.stops_.empty() && data_.stops_.back().offset_ > stop.offset_)
      stop.offset_ = data_.stops_.back().offset_;
    data_.stops_.push_back(stop);
  }

  void set_transform_matrix(const boost::array<double, 6> & matrix)
  { data_.matrix_ = matrix; }

  template<class StringRange>
  void set(svgpp::tag::attribute::id, StringRange const & str)
  {
    id_.assign(boost::begin(str), boost::end(str));
  }

  void set(svgpp::tag::attribute::gradientUnits, svgpp::tag::value::userSpaceOnUse)
  { data_.useObjectBoundingBox_ = false; }

  void set(svgpp::tag::attribute::gradientUnits, svgpp::tag::value::objectBoundingBox)
  { data_.useObjectBoundingBox_ = true; }

  void set(svgpp::tag::attribute::spreadMethod, svgpp::tag::value::pad)
  { data_.spreadMethod_ = GradientBase::spreadPad; }

  void set(svgpp::tag::attribute::spreadMethod, svgpp::tag::value::reflect)
  { data_.spreadMethod_ = GradientBase::spreadReflect; }

  void set(svgpp::tag::attribute::spreadMethod, svgpp::tag::value::repeat)
  { data_.spreadMethod_ = GradientBase::spreadRepeat; }

protected:
  std::string id_;
  GradientBase & data_;
};

void GradientStopContext::on_exit_element()
{
  data_.color_.opacity(opacity_);
  parent_.add_stop(data_);
}

class LinearGradientContext: public GradientBaseContext
{
public:
  LinearGradientContext(GradientContext & gradient)
    : GradientBaseContext(data_)
    , gradient_(gradient)
  {}

  void on_exit_element()
  { gradient_ = data_; }

  using GradientBaseContext::set;

  void set(svgpp::tag::attribute::x1, double val)
  { data_.x1_ = val; }

  void set(svgpp::tag::attribute::y1, double val)
  { data_.y1_ = val; }

  void set(svgpp::tag::attribute::x2, double val)
  { data_.x2_ = val; }

  void set(svgpp::tag::attribute::y2, double val)
  { data_.y2_ = val; }

private:
  GradientContext & gradient_;
  LinearGradient data_;
};

class RadialGradientContext: public GradientBaseContext
{
public:
  RadialGradientContext(GradientContext & gradient)
    : GradientBaseContext(data_)
    , gradient_(gradient)
    , fx_set_(false)
    , fy_set_(false)
  {}

  void on_exit_element()
  { 
    if (!fx_set_)
      data_.fx_ = data_.cx_;
    if (!fy_set_)
      data_.fy_ = data_.cy_;
    gradient_ = data_; 
  }

  using GradientBaseContext::set;

  void set(svgpp::tag::attribute::cx, double val)
  { data_.cx_ = val; }

  void set(svgpp::tag::attribute::cy, double val)
  { data_.cy_ = val; }

  void set(svgpp::tag::attribute::r, double val)
  { data_.r_ = val; }

  void set(svgpp::tag::attribute::fx, double val)
  { fx_set_ = true; data_.fx_ = val; }

  void set(svgpp::tag::attribute::fy, double val)
  { fy_set_ = true; data_.fy_ = val; }

private:
  GradientContext & gradient_;
  RadialGradient data_;
  bool fx_set_, fy_set_;
};

struct gradient_child_context_factories
{
  template<class ParentContext, class ElementTag>
  struct apply;

  template<>
  struct apply<GradientContext, svgpp::tag::element::linearGradient>
  {
    typedef svgpp::child_context_factory_on_stack<GradientContext, LinearGradientContext> type;
  };

  template<>
  struct apply<GradientContext, svgpp::tag::element::radialGradient>
  {
    typedef svgpp::child_context_factory_on_stack<GradientContext, RadialGradientContext> type;
  };

  template<class ParentContext>
  struct apply<ParentContext, svgpp::tag::element::stop>
  {
    typedef svgpp::child_context_factory_on_stack<ParentContext, GradientStopContext> type;
  };
};

}

Gradient const * Gradients::get(std::string const & id)
{
  std::pair<GradientMap::iterator, bool> ins = gradients_.insert(
    GradientMap::value_type(id, boost::optional<Gradient>()));
  if (ins.second)
    load_gradient(id, ins.first->second);
  return ins.first->second.get_ptr();
}

void Gradients::load_gradient(std::string const & id, boost::optional<Gradient> & out) const
{
  // TODO: inheritance via xlink::href
  if (rapidxml_ns::xml_node<> const * node = xml_document_.find_element_by_id(id))
  {
    try
    {
      svgpp::document_traversal<
        svgpp::child_context_factories<gradient_child_context_factories>,
        svgpp::color_factory<color_factory>,
        svgpp::processed_elements<
          boost::mpl::set<
            svgpp::tag::element::linearGradient,
            svgpp::tag::element::radialGradient,
            svgpp::tag::element::stop
          > 
        >,
        svgpp::processed_attributes<
          boost::mpl::set<
            svgpp::tag::attribute::x1,
            svgpp::tag::attribute::y1,
            svgpp::tag::attribute::x2,
            svgpp::tag::attribute::y2,
            svgpp::tag::attribute::fx,
            svgpp::tag::attribute::fy,
            svgpp::tag::attribute::cx,
            svgpp::tag::attribute::cy,
            svgpp::tag::attribute::r,
            svgpp::tag::attribute::gradientUnits,
            svgpp::tag::attribute::gradientTransform,
            svgpp::tag::attribute::spreadMethod,
            svgpp::tag::attribute::offset,
            boost::mpl::pair<svgpp::tag::element::stop, svgpp::tag::attribute::stop_color>,
            boost::mpl::pair<svgpp::tag::element::stop, svgpp::tag::attribute::stop_opacity>
          >
        >
      >::load_referenced_element<svgpp::traits::gradient_elements>(node, out);
    } catch (std::exception const & e)
    {
      std::cerr << "Error loading paint \"" << id << "\": " << e.what() << "\n";
    }
  }
}

