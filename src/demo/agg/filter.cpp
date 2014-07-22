#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_SET_SIZE 40
#include "filter.hpp"

#include <svgpp/document_traversal.hpp>
#include <boost/mpl/set.hpp>
#include <boost/variant.hpp>

namespace mpl = boost::mpl;

struct ElementWithRegion
{
  double x_, y_, width_, height_;
};

struct FilterInput
{
  enum source { fiNotSet, fiSourceGraphic, fiSourceAlpha, fiBackgroundImage, 
    fiBackgroundAlpha, fiFillPaint, fiStrokePaint, fiReference };

  source source_;
  std::string reference_;
};

struct FilterElementBase: ElementWithRegion
{
  std::string result_;
};

struct feBlend: FilterElementBase
{
  enum BlendMode { mNormal, mMultiply, mScreen, mDarken, mLighten };

  FilterInput input1_, input2_;
  BlendMode mode_;
};

struct feFunc
{
  enum type { fIdentity, fTable, fDiscrete, fLinear, fGamma };

  type type_;
  double slope_, intercept_, amplitude_, exponent_, offset_;
  std::vector<double> tableValues_;
};

struct feComponentTransfer: FilterElementBase
{
  enum ARGBComponent { argbA, argbR, argbG, argbB };

  FilterInput input_;
  feFunc func_[4];
};

typedef boost::variant<feBlend, feComponentTransfer> FilterElement;

class ElementWithRegionContext
{
public:
  ElementWithRegionContext(ElementWithRegion & data)
    : data_(data)
  {}

  void set(svgpp::tag::attribute::x, double val)  { data_.x_ = val; }
  void set(svgpp::tag::attribute::y, double val)  { data_.y_ = val; }
  void set(svgpp::tag::attribute::width, double val)  { data_.width_ = val; }
  void set(svgpp::tag::attribute::height, double val) { data_.height_ = val; }

protected:
  ElementWithRegion & data_;
};

template<class InAttributeTag>
class ElementWithInputContext
{
  typedef mpl::map<
    mpl::pair<svgpp::tag::value::SourceGraphic,    mpl::integral_c<FilterInput::source, FilterInput::fiSourceGraphic> >,
    mpl::pair<svgpp::tag::value::SourceAlpha,      mpl::integral_c<FilterInput::source, FilterInput::fiSourceAlpha> >,
    mpl::pair<svgpp::tag::value::BackgroundImage,  mpl::integral_c<FilterInput::source, FilterInput::fiBackgroundImage> >,
    mpl::pair<svgpp::tag::value::BackgroundAlpha,  mpl::integral_c<FilterInput::source, FilterInput::fiBackgroundAlpha> >,
    mpl::pair<svgpp::tag::value::FillPaint,        mpl::integral_c<FilterInput::source, FilterInput::fiFillPaint> >,
    mpl::pair<svgpp::tag::value::StrokePaint,      mpl::integral_c<FilterInput::source, FilterInput::fiStrokePaint> >
  > filter_input_to_enum;

public:
  ElementWithInputContext(FilterInput & data)
    : data_(data)
  {
    data_.source_ = FilterInput::fiNotSet;
  }

  template<class Input>
  void set(InAttributeTag, Input)
  {
    data_.source_ = mpl::at<filter_input_to_enum, Input>::type::value;
  }

  void set(InAttributeTag, boost::iterator_range<svg_string_t::const_pointer> const & reference)
  {
    data_.source_ = FilterInput::fiReference;
    data_.reference_.assign(boost::begin(reference), boost::end(reference));
  }

protected:
  FilterInput & data_;
};

class FilterElementBaseContext: public ElementWithRegionContext
{
public:
  FilterElementBaseContext(FilterElementBase & data)
    : ElementWithRegionContext(data)
    , data_(data)
  {}

  using ElementWithRegionContext::set;

  template<class Range>
  void set(svgpp::tag::attribute::result, Range const & r)
  {
    data_.result_.assign(boost::begin(r), boost::end(r));
  }

protected:
  FilterElementBase & data_;
};

class FilterContext: public ElementWithRegionContext
{
public:
  FilterContext()
    : ElementWithRegionContext(region_)
  {}

  void on_enter_element(svgpp::tag::element::filter) const 
  {}

  void on_exit_element()
  {}

  using ElementWithRegionContext::set;

  void set(svgpp::tag::attribute::filterUnits, svgpp::tag::value::userSpaceOnUse)
  {
    filterUnitsUseObjectBoundingBox_ = false;
  }

  void set(svgpp::tag::attribute::filterUnits, svgpp::tag::value::objectBoundingBox)
  {
    filterUnitsUseObjectBoundingBox_ = true;
  }

  void addElement(FilterElement const & el)
  {
    elements_.push_back(el);
  }

private:
  ElementWithRegion region_;
  bool filterUnitsUseObjectBoundingBox_;
  std::vector<FilterElement> elements_;
};

class feBlendContext: 
  public FilterElementBaseContext,
  public ElementWithInputContext<svgpp::tag::attribute::in>,
  public ElementWithInputContext<svgpp::tag::attribute::in2>
{
  typedef mpl::map<
    mpl::pair< svgpp::tag::value::normal,   mpl::integral_c<feBlend::BlendMode, feBlend::mNormal> >,
    mpl::pair< svgpp::tag::value::multiply, mpl::integral_c<feBlend::BlendMode, feBlend::mMultiply> >,
    mpl::pair< svgpp::tag::value::screen,   mpl::integral_c<feBlend::BlendMode, feBlend::mScreen> >,
    mpl::pair< svgpp::tag::value::darken,   mpl::integral_c<feBlend::BlendMode, feBlend::mDarken> >,
    mpl::pair< svgpp::tag::value::lighten,  mpl::integral_c<feBlend::BlendMode, feBlend::mLighten> >
  > mode_to_enum;

public:
  feBlendContext(FilterContext & parent)
    : FilterElementBaseContext(data_)
    , ElementWithInputContext<svgpp::tag::attribute::in>(data_.input1_)
    , ElementWithInputContext<svgpp::tag::attribute::in2>(data_.input2_)
    , parent_(parent)
  {
  }

  void on_exit_element() const 
  {
    parent_.addElement(data_);
  }

  using FilterElementBaseContext::set;
  using ElementWithInputContext<svgpp::tag::attribute::in>::set;
  using ElementWithInputContext<svgpp::tag::attribute::in2>::set;

  template<class Mode>
  void set(svgpp::tag::attribute::mode, Mode)
  {
    data_.mode_ = mpl::at<mode_to_enum, Mode>::type::value;
  }

private:
  FilterContext & parent_;
  feBlend data_;
};

class feComponentTransferContext;

template<feComponentTransfer::ARGBComponent ComponentArg>
class feFuncContext
{
  typedef mpl::map<
    mpl::pair<svgpp::tag::value::identity,   mpl::integral_c<feFunc::type, feFunc::fIdentity> >,
    mpl::pair<svgpp::tag::value::table,      mpl::integral_c<feFunc::type, feFunc::fTable> >,
    mpl::pair<svgpp::tag::value::discrete,   mpl::integral_c<feFunc::type, feFunc::fDiscrete> >,
    mpl::pair<svgpp::tag::value::linear,     mpl::integral_c<feFunc::type, feFunc::fLinear> >,
    mpl::pair<svgpp::tag::value::gamma,      mpl::integral_c<feFunc::type, feFunc::fGamma> >
  > type_to_enum;

public:
  feFuncContext(feComponentTransferContext & parent)
    : parent_(parent)
  {}

  void on_exit_element() const;

  template<class Type>
  void set(svgpp::tag::attribute::type, Type)
  {
    data_.type_ = mpl::at<type_to_enum, Type>::type::value;
  }

  template<class Range>
  void set(svgpp::tag::attribute::tableValues, Range const & range)
  {
    data_.tableValues_.assign(boost::begin(range), boost::end(range));
  }

  void set(svgpp::tag::attribute::slope, double value) { data_.slope_ = value; }
  void set(svgpp::tag::attribute::intercept, double value) { data_.intercept_ = value; }
  void set(svgpp::tag::attribute::amplitude, double value) { data_.amplitude_ = value; }
  void set(svgpp::tag::attribute::exponent, double value) { data_.exponent_ = value; }
  void set(svgpp::tag::attribute::offset, double value) { data_.offset_ = value; }

protected:
  feComponentTransferContext & parent_;
  feFunc data_;
};

class feComponentTransferContext:
  public FilterElementBaseContext,
  public ElementWithInputContext<svgpp::tag::attribute::in>
{
public:
  feComponentTransferContext(FilterContext & parent)
    : FilterElementBaseContext(data_)
    , ElementWithInputContext<svgpp::tag::attribute::in>(data_.input_)
    , parent_(parent)
  {
  }

  void on_exit_element() const 
  {
    parent_.addElement(data_);
  }

  void setComponentFunction(feComponentTransfer::ARGBComponent component, feFunc const & func)
  {
    data_.func_[component] = func;
  }

  using FilterElementBaseContext::set;
  using ElementWithInputContext<svgpp::tag::attribute::in>::set;

private:
  FilterContext & parent_;
  feComponentTransfer data_;
};

template<feComponentTransfer::ARGBComponent ComponentArg>
void feFuncContext<ComponentArg>::on_exit_element() const
{
  parent_.setComponentFunction(ComponentArg, data_);
}

struct context_factories
{
  template<class ParentContext, class ElementTag>
  struct apply;
};

template<>
struct context_factories::apply<FilterContext, svgpp::tag::element::filter>
{
  typedef svgpp::context_factory::same<FilterContext, svgpp::tag::element::filter> type;
};

template<>
struct context_factories::apply<FilterContext, svgpp::tag::element::feBlend>
{
  typedef svgpp::context_factory::on_stack<FilterContext, feBlendContext> type;
};

template<>
struct context_factories::apply<FilterContext, svgpp::tag::element::feComponentTransfer>
{
  typedef svgpp::context_factory::on_stack<FilterContext, feComponentTransferContext> type;
};

template<>
struct context_factories::apply<feComponentTransferContext, svgpp::tag::element::feFuncA>
{
  typedef svgpp::context_factory::on_stack<feComponentTransferContext, feFuncContext<feComponentTransfer::argbA> > type;
};

template<>
struct context_factories::apply<feComponentTransferContext, svgpp::tag::element::feFuncR>
{
  typedef svgpp::context_factory::on_stack<feComponentTransferContext, feFuncContext<feComponentTransfer::argbR> > type;
};

template<>
struct context_factories::apply<feComponentTransferContext, svgpp::tag::element::feFuncG>
{
  typedef svgpp::context_factory::on_stack<feComponentTransferContext, feFuncContext<feComponentTransfer::argbG> > type;
};

template<>
struct context_factories::apply<feComponentTransferContext, svgpp::tag::element::feFuncB>
{
  typedef svgpp::context_factory::on_stack<feComponentTransferContext, feFuncContext<feComponentTransfer::argbB> > type;
};

void Filters::get(svg_string_t const & id, length_factory_t const &)
{
  if (XMLElement node = xml_document_.findElementById(id))
  {
    FilterContext filter_context;
    svgpp::document_traversal<
      svgpp::context_factories<context_factories>,
      svgpp::processed_elements<
        boost::mpl::set<
          svgpp::tag::element::filter,
          svgpp::tag::element::feMergeNode,
          svgpp::tag::element::feDistantLight,
          svgpp::tag::element::fePointLight,
          svgpp::tag::element::feSpotLight,
          svgpp::tag::element::feComponentTransfer,
          svgpp::tag::element::feFuncA,
          svgpp::tag::element::feFuncB,
          svgpp::tag::element::feFuncG,
          svgpp::tag::element::feFuncR
        >
      >,
      svgpp::processed_attributes<
        boost::mpl::set<
          boost::mpl::pair<svgpp::tag::element::filter, svgpp::tag::attribute::x>,
          boost::mpl::pair<svgpp::tag::element::filter, svgpp::tag::attribute::y>,
          boost::mpl::pair<svgpp::tag::element::filter, svgpp::tag::attribute::width>,
          boost::mpl::pair<svgpp::tag::element::filter, svgpp::tag::attribute::height>,
          boost::mpl::pair<svgpp::tag::element::filter, svgpp::tag::attribute::filterUnits>,
          //boost::mpl::pair<svgpp::tag::element::filter, svgpp::tag::attribute::primitiveUnits>,

          svgpp::tag::attribute::result,
          svgpp::tag::attribute::in,
          svgpp::tag::attribute::in2,
          boost::mpl::pair<svgpp::tag::element::feBlend, svgpp::tag::attribute::x>,
          boost::mpl::pair<svgpp::tag::element::feBlend, svgpp::tag::attribute::y>,
          boost::mpl::pair<svgpp::tag::element::feBlend, svgpp::tag::attribute::width>,
          boost::mpl::pair<svgpp::tag::element::feBlend, svgpp::tag::attribute::height>,
          boost::mpl::pair<svgpp::tag::element::feBlend, svgpp::tag::attribute::mode>,
          boost::mpl::pair<svgpp::tag::element::feColorMatrix, svgpp::tag::attribute::type>,
          boost::mpl::pair<svgpp::tag::element::feColorMatrix, svgpp::tag::attribute::values>,

          // transfer function element attributes
          boost::mpl::pair<svgpp::tag::element::feFuncA, svgpp::tag::attribute::type>,
          boost::mpl::pair<svgpp::tag::element::feFuncR, svgpp::tag::attribute::type>,
          boost::mpl::pair<svgpp::tag::element::feFuncG, svgpp::tag::attribute::type>,
          boost::mpl::pair<svgpp::tag::element::feFuncB, svgpp::tag::attribute::type>,
          svgpp::tag::attribute::tableValues,
          svgpp::tag::attribute::slope, 
          svgpp::tag::attribute::intercept, 
          svgpp::tag::attribute::amplitude, 
          svgpp::tag::attribute::exponent, 
          svgpp::tag::attribute::offset
        >
      >
    >::load_referenced_element<
      svgpp::expected_elements<boost::mpl::set1<svgpp::tag::element::filter> >
    >::load(node, filter_context);
  }
  else
    throw std::runtime_error("Filter not found");
}