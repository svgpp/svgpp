#include "gradient.hpp"

#include <svgpp/document_traversal.hpp>
#include <boost/mpl/set.hpp>

namespace
{

class GradientBaseContext;

class GradientContext
{
public:
  GradientContext(
    length_factory_t const & length_factory/*,
    get_bounding_box_func_t const & get_bounding_box*/)
    : referenced_length_factory_(length_factory)
    //, get_bounding_box_(get_bounding_box)
  {}

  length_factory_t const & referenced_length_factory_; 
  //get_bounding_box_func_t const & get_bounding_box_;*/
  boost::optional<Gradient> gradient_;
};

struct afterGradientUnitsTag {};

class GradientStopContext
{
public:
  GradientStopContext(GradientBaseContext & parent)
    : parent_(parent)
    , opacity_(1)
  {}

  void on_exit_element();

  void set(svgpp::tag::attribute::offset, number_t val)
  { data_.offset_ = std::min(number_t(1), std::max(number_t(0), val)); }

  void set(svgpp::tag::attribute::stop_color, svgpp::tag::value::inherit)
  {} // TODO

  void set(svgpp::tag::attribute::stop_color, svgpp::tag::value::currentColor)
  {} // TODO

  void set(svgpp::tag::attribute::stop_color, color_t color, svgpp::tag::skip_icc_color = svgpp::tag::skip_icc_color())
  { data_.color_ = color; }

  void set(svgpp::tag::attribute::stop_opacity, number_t val)
  { opacity_ = std::min(number_t(1), std::max(number_t(0), val)); }

  void set(svgpp::tag::attribute::stop_opacity, svgpp::tag::value::inherit)
  {} // TODO

private:
  GradientBaseContext & parent_;
  GradientStop data_;
  number_t opacity_;
};

class GradientBaseContext
{
public:
  GradientBaseContext(GradientBase & data, GradientContext const & gradientContext)
    : data_(data)
    , gradientContext_(gradientContext)
  {}

  length_factory_t const & length_factory()
  {
    return length_factory_;
  }

  void addStop(GradientStop stop)
  {
    if (!data_.stops_.empty() && data_.stops_.back().offset_ > stop.offset_)
      stop.offset_ = data_.stops_.back().offset_;
    data_.stops_.push_back(stop);
  }

  void transform_matrix(const boost::array<number_t, 6> & matrix)
  { 
#if defined(RENDERER_SKIA)
    SkMatrix m;
    m.setAffine(matrix.data());
    data_.matrix_ = m;
#else
    data_.matrix_ = matrix; 
#endif
  }

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
#if defined(RENDERER_SKIA)
  { data_.spreadMethod_ = SkShader::kClamp_TileMode; }
#else
  { data_.spreadMethod_ = GradientBase::spreadPad; }
#endif

  void set(svgpp::tag::attribute::spreadMethod, svgpp::tag::value::reflect)
#if defined(RENDERER_SKIA)
  { data_.spreadMethod_ = SkShader::kMirror_TileMode; }
#else
  { data_.spreadMethod_ = GradientBase::spreadReflect; }
#endif

  void set(svgpp::tag::attribute::spreadMethod, svgpp::tag::value::repeat)
#if defined(RENDERER_SKIA)
  { data_.spreadMethod_ = SkShader::kRepeat_TileMode; }
#else
  { data_.spreadMethod_ = GradientBase::spreadRepeat; }
#endif

  bool notify(afterGradientUnitsTag)
  { 
    if (!data_.useObjectBoundingBox_)
      length_factory_ = gradientContext_.referenced_length_factory_;
    else
      length_factory_.set_viewport_size(1.0, 1.0);
    return true; 
  }

protected:
  svg_string_t id_;
  GradientBase & data_;
  GradientContext const & gradientContext_;
  length_factory_t length_factory_;
};

void GradientStopContext::on_exit_element()
{
#if defined(RENDERER_AGG)
  data_.color_.opacity(opacity_);
#elif defined(RENDERER_GDIPLUS)
  data_.color_ = Gdiplus::Color(opacity_ * 255, data_.color_.GetR(), data_.color_.GetG(), data_.color_.GetB());
#elif defined(RENDERER_SKIA)
  data_.color_ = SkColorSetA(data_.color_, opacity_ * 255);
#endif
  parent_.addStop(data_);
}

class LinearGradientContext: public GradientBaseContext
{
public:
  LinearGradientContext(GradientContext & gradientContext)
    : GradientBaseContext(data_, gradientContext)
    , gradientContext_(gradientContext)
    , x2_set_(false)
  {}

  void on_exit_element()
  { 
    if (!x2_set_)
      data_.x2_ = length_factory().create_length(
        100, svgpp::tag::length_units::percent(), svgpp::tag::length_dimension::width());
    gradientContext_.gradient_ = data_; 
  }

  using GradientBaseContext::set;

  void set(svgpp::tag::attribute::x1, number_t val)
  { data_.x1_ = val; }

  void set(svgpp::tag::attribute::y1, number_t val)
  { data_.y1_ = val; }

  void set(svgpp::tag::attribute::x2, number_t val)
  { data_.x2_ = val; x2_set_ = true; }

  void set(svgpp::tag::attribute::y2, number_t val)
  { data_.y2_ = val; }

private:
  GradientContext & gradientContext_;
  LinearGradient data_;
  bool x2_set_;
};

class RadialGradientContext: public GradientBaseContext
{
public:
  RadialGradientContext(GradientContext & gradientContext)
    : GradientBaseContext(data_, gradientContext)
    , gradientContext_(gradientContext)
    , cx_set_(false)
    , cy_set_(false)
    , r_set_ (false)
    , fx_set_(false)
    , fy_set_(false)
  {}

  void on_exit_element()
  { 
    if (!cx_set_)
      data_.cx_ = length_factory().create_length(
        50, svgpp::tag::length_units::percent(), svgpp::tag::length_dimension::width());
    if (!cy_set_)
      data_.cy_ = length_factory().create_length(
        50, svgpp::tag::length_units::percent(), svgpp::tag::length_dimension::height());
    if (!r_set_)
      data_.r_ = length_factory().create_length(
        50, svgpp::tag::length_units::percent(), svgpp::tag::length_dimension::not_width_nor_height());
    if (!fx_set_)
      data_.fx_ = data_.cx_;
    if (!fy_set_)
      data_.fy_ = data_.cy_;
    gradientContext_.gradient_ = data_; 
  }

  using GradientBaseContext::set;

  void set(svgpp::tag::attribute::cx, number_t val)
  { cx_set_ = true; data_.cx_ = val; }

  void set(svgpp::tag::attribute::cy, number_t val)
  { cy_set_ = true; data_.cy_ = val; }

  void set(svgpp::tag::attribute::r, number_t val)
  { r_set_ = true; data_.r_ = val; }

  void set(svgpp::tag::attribute::fx, number_t val)
  { fx_set_ = true; data_.fx_ = val; }

  void set(svgpp::tag::attribute::fy, number_t val)
  { fy_set_ = true; data_.fy_ = val; }

private:
  GradientContext & gradientContext_;
  RadialGradient data_;
  bool cx_set_, cy_set_, r_set_;
  bool fx_set_, fy_set_;
};

struct gradient_context_factories
{
  template<class ParentContext, class ElementTag>
  struct apply;
};

template<>
struct gradient_context_factories::apply<GradientContext, svgpp::tag::element::linearGradient>
{
  typedef svgpp::factory::context::on_stack<LinearGradientContext> type;
};

template<>
struct gradient_context_factories::apply<GradientContext, svgpp::tag::element::radialGradient>
{
  typedef svgpp::factory::context::on_stack<RadialGradientContext> type;
};

template<class ParentContext>
struct gradient_context_factories::apply<ParentContext, svgpp::tag::element::stop>
{
  typedef svgpp::factory::context::on_stack<GradientStopContext> type;
};

}

struct attribute_traversal: svgpp::policy::attribute_traversal::default_policy
{
  typedef boost::mpl::if_<
    boost::mpl::has_key<svgpp::traits::gradient_elements, boost::mpl::_1>,
    boost::mpl::vector<
      svgpp::tag::attribute::gradientUnits,
      svgpp::notify_context<afterGradientUnitsTag>
    >,
    boost::mpl::empty_sequence
  > get_priority_attributes_by_element;
};

boost::optional<Gradient> Gradients::get(
    svg_string_t const & id, 
    length_factory_t const & length_factory/*, 
    get_bounding_box_func_t const & get_bounding_box*/)
{
  // TODO: inheritance via xlink::href
  if (XMLElement node = xml_document_.findElementById(id))
  {
    try
    {
      GradientContext gradient_context(length_factory);
      svgpp::document_traversal<
        svgpp::number_type<number_t>,
        svgpp::context_factories<gradient_context_factories>,
        svgpp::color_factory<color_factory_t>,
        svgpp::attribute_traversal_policy<attribute_traversal>,
        svgpp::length_policy<svgpp::policy::length::forward_to_method<GradientBaseContext, const length_factory_t> >,
        svgpp::transform_events_policy<svgpp::policy::transform_events::forward_to_method<GradientBaseContext> >, // Same as default, but less instantiations
        svgpp::processed_elements<
          boost::mpl::set<
            svgpp::tag::element::linearGradient,
            svgpp::tag::element::radialGradient,
            svgpp::tag::element::stop
          >::type 
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
          >::type
        >
      >::load_referenced_element<
        svgpp::expected_elements<svgpp::traits::gradient_elements>
      >::load(node, gradient_context);
      return gradient_context.gradient_;
    } catch (std::exception const & e)
    {
      std::cerr << "Error loading paint \"" << std::string(id.begin(), id.end()) << "\": " << e.what() << "\n";
    }
  }

  return boost::optional<Gradient>();
}
