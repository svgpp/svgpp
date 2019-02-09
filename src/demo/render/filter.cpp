#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_SET_SIZE 40

#include "filter.hpp"

#include <svgpp/document_traversal.hpp>
#include <svgpp/utility/gil/blend.hpp>
#include <svgpp/utility/gil/composite.hpp>
#include <svgpp/utility/gil/color_matrix.hpp>
#include <svgpp/utility/gil/mask.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/mpl/set.hpp>
#include <boost/noncopyable.hpp>
#include <boost/variant.hpp>
#include <boost/math/constants/constants.hpp>

namespace mpl = boost::mpl;
namespace gil = boost::gil;

struct ElementWithRegion
{
  double x_, y_, width_, height_;
};

struct FilterInput
{
  enum source { fiNotSet, fiSourceGraphic, fiSourceAlpha, fiBackgroundImage, 
    fiBackgroundAlpha, fiFillPaint, fiStrokePaint, fiReference };

  FilterInput()
    : source_(fiNotSet)
  {}

  source source_;
  svg_string_t reference_;
};

struct FilterElementBase: ElementWithRegion
{
  svg_string_t result_;
};

struct feBlend: FilterElementBase
{
  enum BlendMode { mNormal, mMultiply, mScreen, mDarken, mLighten };

  FilterInput input1_, input2_;
  BlendMode mode_;
};

struct feFunc
{
  enum Type { fIdentity, fTable, fDiscrete, fLinear, fGamma };

  feFunc()
    : type_(fIdentity)
  {}

  Type type_;
  double slope_, intercept_, amplitude_, exponent_, offset_;
  std::vector<double> tableValues_;
};

struct feComponentTransfer: FilterElementBase
{
  enum ARGBComponent { argbR, argbG, argbB, argbA }; // Must be the same order as channels in GIL color

  FilterInput input_;
  feFunc func_[4];
};

struct feOffset: FilterElementBase
{
  feOffset()
    : dx_(0), dy_(0)
  {}

  FilterInput input_;
  double dx_, dy_;
};

struct feComposite: FilterElementBase
{
  enum Operator { opOver, opIn, opOut, opAtop, opXor, opArithmetic };

  feComposite()
    : operator_(opOver)
    , k1_(0), k2_(0), k3_(0), k4_(0)
  {}

  FilterInput input1_, input2_;
  Operator operator_;
  double k1_, k2_, k3_, k4_;
};

struct feMerge: FilterElementBase
{
  std::vector<FilterInput> inputs_;
};

struct feColorMatrix: FilterElementBase
{
  enum Type { mMatrix, mSaturate, mHueRotate, mLuminanceToAlpha };

  feColorMatrix()
    : type_(mMatrix)
  {}

  FilterInput input_;
  Type type_;
  boost::optional<std::vector<double> > values_;
};


typedef boost::variant<feBlend, feComponentTransfer, feOffset, feComposite, feMerge,
  feColorMatrix> FilterElement;

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
    mpl::pair<svgpp::tag::value::identity,   mpl::integral_c<feFunc::Type, feFunc::fIdentity> >,
    mpl::pair<svgpp::tag::value::table,      mpl::integral_c<feFunc::Type, feFunc::fTable> >,
    mpl::pair<svgpp::tag::value::discrete,   mpl::integral_c<feFunc::Type, feFunc::fDiscrete> >,
    mpl::pair<svgpp::tag::value::linear,     mpl::integral_c<feFunc::Type, feFunc::fLinear> >,
    mpl::pair<svgpp::tag::value::gamma,      mpl::integral_c<feFunc::Type, feFunc::fGamma> >
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

class feOffsetContext:
  public FilterElementBaseContext,
  public ElementWithInputContext<svgpp::tag::attribute::in>
{
public:
  feOffsetContext(FilterContext & parent)
    : FilterElementBaseContext(data_)
    , ElementWithInputContext<svgpp::tag::attribute::in>(data_.input_)
    , parent_(parent)
  {
  }

  void on_exit_element() const 
  {
    parent_.addElement(data_);
  }

  using FilterElementBaseContext::set;
  using ElementWithInputContext<svgpp::tag::attribute::in>::set;

  void set(svgpp::tag::attribute::dx, double val)
  { data_.dx_ = val; }

  void set(svgpp::tag::attribute::dy, double val)
  { data_.dy_ = val; }

private:
  FilterContext & parent_;
  feOffset data_;
};

class feCompositeContext: 
  public FilterElementBaseContext,
  public ElementWithInputContext<svgpp::tag::attribute::in>,
  public ElementWithInputContext<svgpp::tag::attribute::in2>
{
  typedef mpl::map<
    mpl::pair< svgpp::tag::value::over,       mpl::integral_c<feComposite::Operator, feComposite::opOver> >,
    mpl::pair< svgpp::tag::value::in,         mpl::integral_c<feComposite::Operator, feComposite::opIn> >,
    mpl::pair< svgpp::tag::value::out,        mpl::integral_c<feComposite::Operator, feComposite::opOut> >,
    mpl::pair< svgpp::tag::value::atop,       mpl::integral_c<feComposite::Operator, feComposite::opAtop> >,
    mpl::pair< svgpp::tag::value::xor_,       mpl::integral_c<feComposite::Operator, feComposite::opXor> >,
    mpl::pair< svgpp::tag::value::arithmetic, mpl::integral_c<feComposite::Operator, feComposite::opArithmetic> >
  > op_to_enum;

public:
  feCompositeContext(FilterContext & parent)
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

  template<class Op>
  void set(svgpp::tag::attribute::operator_, Op)
  {
    data_.operator_ = mpl::at<op_to_enum, Op>::type::value;
  }

  void set(svgpp::tag::attribute::k1, double val)
  { data_.k1_ = val; }

  void set(svgpp::tag::attribute::k2, double val)
  { data_.k2_ = val; }

  void set(svgpp::tag::attribute::k3, double val)
  { data_.k3_ = val; }

  void set(svgpp::tag::attribute::k4, double val)
  { data_.k4_ = val; }

private:
  FilterContext & parent_;
  feComposite data_;
};

class feMergeContext: 
  public FilterElementBaseContext
{
public:
  feMergeContext(FilterContext & parent)
    : FilterElementBaseContext(data_)
    , parent_(parent)
  {
  }

  void on_exit_element() const 
  {
    parent_.addElement(data_);
  }

private:
  FilterContext & parent_;
  feMerge data_;

  friend class feMergeNodeContext;
};

class feMergeNodeContext:
  public ElementWithInputContext<svgpp::tag::attribute::in>
{
public:
  feMergeNodeContext(feMergeContext & parent)
    : ElementWithInputContext<svgpp::tag::attribute::in>(data_)
    , parent_(parent)
  {}

  void on_exit_element() const 
  {
    parent_.data_.inputs_.push_back(data_);
  }

private:
  feMergeContext & parent_;
  FilterInput data_;
};

class feColorMatrixContext: 
  public FilterElementBaseContext,
  public ElementWithInputContext<svgpp::tag::attribute::in>
{
  typedef mpl::map<
    mpl::pair< svgpp::tag::value::matrix,           mpl::integral_c<feColorMatrix::Type, feColorMatrix::mMatrix> >,
    mpl::pair< svgpp::tag::value::saturate,         mpl::integral_c<feColorMatrix::Type, feColorMatrix::mSaturate> >,
    mpl::pair< svgpp::tag::value::hueRotate,        mpl::integral_c<feColorMatrix::Type, feColorMatrix::mHueRotate> >,
    mpl::pair< svgpp::tag::value::luminanceToAlpha, mpl::integral_c<feColorMatrix::Type, feColorMatrix::mLuminanceToAlpha> >
  > type_to_enum;

public:
  feColorMatrixContext(FilterContext & parent)
    : FilterElementBaseContext(data_)
    , ElementWithInputContext<svgpp::tag::attribute::in>(data_.input_)
    , parent_(parent)
  {
  }

  void on_exit_element() const 
  {
    parent_.addElement(data_);
  }

  using FilterElementBaseContext::set;
  using ElementWithInputContext<svgpp::tag::attribute::in>::set;

  template<class Type>
  void set(svgpp::tag::attribute::type, Type)
  {
    data_.type_ = mpl::at<type_to_enum, Type>::type::value;
  }

  template<class Values>
  void set(svgpp::tag::attribute::values, Values const & values)
  {
    data_.values_ = std::vector<double>(boost::begin(values), boost::end(values));
  }

private:
  FilterContext & parent_;
  feColorMatrix data_;
};

struct context_factories
{
  template<class ParentContext, class ElementTag>
  struct apply;
};

template<>
struct context_factories::apply<FilterContext, svgpp::tag::element::feBlend>
{
  typedef svgpp::factory::context::on_stack<feBlendContext> type;
};

template<>
struct context_factories::apply<FilterContext, svgpp::tag::element::feOffset>
{
  typedef svgpp::factory::context::on_stack<feOffsetContext> type;
};

template<>
struct context_factories::apply<FilterContext, svgpp::tag::element::feComposite>
{
  typedef svgpp::factory::context::on_stack<feCompositeContext> type;
};

template<>
struct context_factories::apply<FilterContext, svgpp::tag::element::feComponentTransfer>
{
  typedef svgpp::factory::context::on_stack<feComponentTransferContext> type;
};

template<>
struct context_factories::apply<FilterContext, svgpp::tag::element::feMerge>
{
  typedef svgpp::factory::context::on_stack<feMergeContext> type;
};

template<>
struct context_factories::apply<feMergeContext, svgpp::tag::element::feMergeNode>
{
  typedef svgpp::factory::context::on_stack<feMergeNodeContext> type;
};

template<>
struct context_factories::apply<FilterContext, svgpp::tag::element::feColorMatrix>
{
  typedef svgpp::factory::context::on_stack<feColorMatrixContext> type;
};

template<>
struct context_factories::apply<feComponentTransferContext, svgpp::tag::element::feFuncA>
{
  typedef svgpp::factory::context::on_stack<feFuncContext<feComponentTransfer::argbA> > type;
};

template<>
struct context_factories::apply<feComponentTransferContext, svgpp::tag::element::feFuncR>
{
  typedef svgpp::factory::context::on_stack<feFuncContext<feComponentTransfer::argbR> > type;
};

template<>
struct context_factories::apply<feComponentTransferContext, svgpp::tag::element::feFuncG>
{
  typedef svgpp::factory::context::on_stack<feFuncContext<feComponentTransfer::argbG> > type;
};

template<>
struct context_factories::apply<feComponentTransferContext, svgpp::tag::element::feFuncB>
{
  typedef svgpp::factory::context::on_stack<feFuncContext<feComponentTransfer::argbB> > type;
};

class BlendView: public IFilterView
{
public:
  BlendView(feBlend const & fe, IFilterViewPtr const & in1, IFilterViewPtr const & in2)
    : fe_(fe)
    , in1_(in1)
    , in2_(in2)
  {}

  virtual gil::rgba8c_view_t view() 
  {
    if (in1_)
    {
      image_.recreate(in1_->view().dimensions());
      switch(fe_.mode_)
      {
      case feBlend::mNormal:
        gil::transform_pixels(in1_->view(), in2_->view(), gil::view(image_), 
          svgpp::gil_utility::blend_pixel<svgpp::tag::value::normal>());
        break;
      case feBlend::mMultiply:
        gil::transform_pixels(in1_->view(), in2_->view(), gil::view(image_),
          svgpp::gil_utility::blend_pixel<svgpp::tag::value::multiply>());
        break;
      case feBlend::mScreen:
        gil::transform_pixels(in1_->view(), in2_->view(), gil::view(image_),
          svgpp::gil_utility::blend_pixel<svgpp::tag::value::screen>());
        break;
      case feBlend::mDarken:
        gil::transform_pixels(in1_->view(), in2_->view(), gil::view(image_),
          svgpp::gil_utility::blend_pixel<svgpp::tag::value::darken>());
        break;
      case feBlend::mLighten:
        gil::transform_pixels(in1_->view(), in2_->view(), gil::view(image_), 
          svgpp::gil_utility::blend_pixel<svgpp::tag::value::lighten>());
        break;
      default:
        BOOST_ASSERT(false);
      }
      in1_.reset();
      in2_.reset();
    }
    return gil::const_view(image_);
  }

private:
  feBlend const fe_;
  IFilterViewPtr in1_, in2_;
  boost::gil::rgba8_image_t image_;
};

class CompositeView: public IFilterView
{
public:
  CompositeView(feComposite const & fe, IFilterViewPtr const & in1, IFilterViewPtr const & in2)
    : fe_(fe)
    , in1_(in1)
    , in2_(in2)
  {}

  virtual gil::rgba8c_view_t view() 
  {
    if (in1_)
    {
      image_.recreate(in1_->view().dimensions());
      switch(fe_.operator_)
      {
      case feComposite::opOver:
        gil::transform_pixels(in1_->view(), in2_->view(), gil::view(image_), 
          svgpp::gil_utility::composite_pixel<svgpp::tag::value::over>());
        break;
      case feComposite::opIn:
        gil::transform_pixels(in1_->view(), in2_->view(), gil::view(image_), 
          svgpp::gil_utility::composite_pixel<svgpp::tag::value::in>());
        break;
      case feComposite::opOut:
        gil::transform_pixels(in1_->view(), in2_->view(), gil::view(image_), 
          svgpp::gil_utility::composite_pixel<svgpp::tag::value::out>());
        break;
      case feComposite::opAtop:
        gil::transform_pixels(in1_->view(), in2_->view(), gil::view(image_), 
          svgpp::gil_utility::composite_pixel<svgpp::tag::value::atop>());
        break;
      case feComposite::opXor:
        gil::transform_pixels(in1_->view(), in2_->view(), gil::view(image_), 
          svgpp::gil_utility::composite_pixel<svgpp::tag::value::xor_>());
        break;
      case feComposite::opArithmetic:
        gil::transform_pixels(in1_->view(), in2_->view(), gil::view(image_), 
          svgpp::gil_utility::composite_pixel_arithmetic<boost::gil::rgba8c_view_t::value_type>(fe_.k1_, fe_.k2_, fe_.k3_, fe_.k4_));
        break;
      default:
        BOOST_ASSERT(false);
      }
      in1_.reset();
      in2_.reset();
    }
    return gil::const_view(image_);
  }

private:
  feComposite const fe_;
  IFilterViewPtr in1_, in2_;
  boost::gil::rgba8_image_t image_;
};

typedef boost::array<boost::uint8_t, 256> ChannelTransferTable;

struct ComponentTransferPixel
{
  ComponentTransferPixel(ChannelTransferTable const * tables)
    : tables_(tables)
  {}

  gil::rgba8_pixel_t operator()(const gil::rgba8_pixel_t & src) const 
  {
    gil::rgba8_pixel_t dst;
    for(size_t ch = 0; ch < 4; ++ch)
      dst[ch] = tables_[ch][src[ch]];

    return dst;
  }

private:
  ChannelTransferTable const * tables_;
};

class ComponentTransferView: public IFilterView
{
public:
  ComponentTransferView(feComponentTransfer const & fe, IFilterViewPtr const & in)
    : fe_(fe)
    , in_(in)
  {}

  virtual gil::rgba8c_view_t view() 
  {
    if (in_)
    {
      image_.recreate(in_->view().dimensions());
      ChannelTransferTable tables[4];
      for(int ch = 0; ch < 4; ++ch)
      {
        ChannelTransferTable & table = tables[ch];
        feFunc const & func = fe_.func_[ch];
        switch(func.type_)
        {
        case feFunc::fTable:
        case feFunc::fDiscrete: // TODO
        case feFunc::fIdentity:
          for(int i=0; i<256; ++i)
            table[i] = i;
          break;
        case feFunc::fLinear:
          // C' = slope * C + intercept
          for(int i=0; i<256; ++i)
            table[i] = svgpp::gil_detail::clamp_channel_bits8(static_cast<int>(func.slope_ * i + func.intercept_ * 255 + 0.49));
          break;
        case feFunc::fGamma:
          for(int i=0; i<256; ++i)
            // C' = amplitude * pow(C, exponent) + offset
            table[i] = svgpp::gil_detail::clamp_channel_bits8(static_cast<int>(
              (func.amplitude_ * std::pow(i / 255.0, func.exponent_) + func.offset_) * 255.0 + 0.49));
          break;
        default:
          BOOST_ASSERT(false);
        }
      }
      gil::transform_pixels(in_->view(), gil::view(image_), ComponentTransferPixel(tables));
      in_.reset();
    }
    return gil::const_view(image_);
  }

private:
  feComponentTransfer const fe_;
  IFilterViewPtr in_;
  boost::gil::rgba8_image_t image_;
};

class OffsetView: public IFilterView
{
public:
  OffsetView(feOffset const & fe, IFilterViewPtr const & in)
    : fe_(fe)
    , in_(in)
  {}

  virtual gil::rgba8c_view_t view() 
  {
    // TODO: use transform
    if (in_)
    {
      image_.recreate(in_->view().dimensions(), gil::rgba8_pixel_t(0, 0, 0, 0), 0);
      int w = image_.width() - std::abs(fe_.dx_);
      int h = image_.height() - std::abs(fe_.dy_);
      if (w > 0 && h > 0)
      {
        gil::copy_pixels(
          gil::subimage_view(in_->view(),  
            std::max(0.0, -fe_.dx_), std::max(0.0, -fe_.dy_), w, h),
          gil::subimage_view(gil::view(image_),  
            std::max(0.0, fe_.dx_), std::max(0.0, fe_.dy_), w, h));
      }
      in_.reset();
    }
    return gil::const_view(image_);
  }

private:
  feOffset const fe_;
  IFilterViewPtr in_;
  boost::gil::rgba8_image_t image_;
};

class MergeView: public IFilterView
{
public:
  void addNode(IFilterViewPtr const & in)
  {
    nodes_.push_back(in);
  }

  virtual gil::rgba8c_view_t view() 
  {
    // TODO: what if empty?
    if (!nodes_.empty())
    {
      image_.recreate(nodes_.front()->view().dimensions());
      if (nodes_.size() == 1)
        gil::copy_pixels(nodes_.front()->view(), gil::view(image_));
      else
      {
        gil::transform_pixels(nodes_[0]->view(), nodes_[1]->view(), gil::view(image_), 
          svgpp::gil_utility::composite_pixel<svgpp::tag::value::over>());
        for(size_t i = 2; i < nodes_.size(); ++i)
          gil::transform_pixels(gil::view(image_), nodes_[i]->view(), gil::view(image_), 
            svgpp::gil_utility::composite_pixel<svgpp::tag::value::over>());
      }
      nodes_.clear();
    }
    return gil::const_view(image_);
  }

private:
  std::vector<IFilterViewPtr> nodes_;
  boost::gil::rgba8_image_t image_;
};

class ColorMatrixView: public IFilterView
{
public:
  ColorMatrixView(feColorMatrix const & fe, IFilterViewPtr const & input)
    : fe_(fe)
    , in_(input)
  {}

  virtual gil::rgba8c_view_t view() 
  {
    if (in_)
    {
      typedef svgpp::gil_utility::color_matrix_transform<boost::gil::rgba8c_view_t::value_type> transform_t;
      image_.recreate(in_->view().dimensions());
      switch(fe_.type_)
      {
      case feColorMatrix::mMatrix:
      {
        if (fe_.values_ && fe_.values_->size() != 20)
          throw std::runtime_error("For feColorMatrix type=\"matrix\", 'values' must be a list of 20 values");
        boost::multi_array<double, 2> m(boost::extents[4][5]);
        if (fe_.values_)
          m.assign(fe_.values_->begin(), fe_.values_->end());
        else
          for(int i=0; i<4; ++i)
            m[i][i] = 1;
        gil::transform_pixels(in_->view(), gil::view(image_), transform_t(m));
      }
      break;
      case feColorMatrix::mSaturate:
      {
        if (fe_.values_ && fe_.values_->size() != 1)
          throw std::runtime_error("For feColorMatrix type=\"saturate\", 'values' must be single real number");
        double saturate = fe_.values_ ? fe_.values_->front() : 1;
        saturate = std::min(1.0, std::max(0.0, saturate));
        gil::transform_pixels(in_->view(), gil::view(image_), 
          transform_t(svgpp::gil_utility::get_saturate_matrix(saturate)));
      }
      break;
      case feColorMatrix::mHueRotate:
      {
        if (fe_.values_ && fe_.values_->size() != 1)
          throw std::runtime_error("For feColorMatrix type=\"hueRotate\", 'values' must be single real number");
        double angle = fe_.values_ ? fe_.values_->front() : 0;
        gil::transform_pixels(in_->view(), gil::view(image_), 
          transform_t(svgpp::gil_utility::get_hue_rotate_matrix(angle * boost::math::constants::degree<double>())));
      }
      break;
      case feColorMatrix::mLuminanceToAlpha:
      {
        gil::copy_pixels(
          gil::color_converted_view<gil::rgba8_pixel_t>(
            in_->view(),
            svgpp::gil_utility::rgba_to_mask_color_converter<boost::gil::alpha_t>()
          ),
          gil::view(image_));
      }
      break;
      }
    }
    return gil::const_view(image_);
  }

private:
  IFilterViewPtr in_;
  feColorMatrix const fe_;
  boost::gil::rgba8_image_t image_;
};

class AlphaChannelView: public IFilterView
{
public:
  AlphaChannelView(IFilterViewPtr const & in)
    : in_(in)
  {}

  virtual gil::rgba8c_view_t view() 
  {
    if (in_)
    {
      image_.recreate(in_->view().dimensions(), gil::rgba8_pixel_t(0, 0, 0, 0), 0);
      gil::copy_pixels(
        gil::kth_channel_view<gil::color_index_type<gil::rgba8_pixel_t, gil::alpha_t>::value>(in_->view()), 
        gil::kth_channel_view<gil::color_index_type<gil::rgba8_pixel_t, gil::alpha_t>::value>(gil::view(image_)));
      in_.reset();
    }
    return gil::const_view(image_);
  }

private:
  IFilterViewPtr in_;
  boost::gil::rgba8_image_t image_;
};

struct FilterElementVisitor:
  public boost::static_visitor<>,
  boost::noncopyable
{
  FilterElementVisitor(Filters::Input const & input)
    : input_(input)
  {}

  void operator()(feBlend const & fe)
  {
    boost::shared_ptr<BlendView> feView(new BlendView(fe, findInput(fe.input1_), findInput(fe.input2_)));
    if (!fe.result_.empty())
      namedFilters_[fe.result_] = feView;
    lastFilter_ = feView;
  }

  void operator()(feComponentTransfer const & fe)
  {
    boost::shared_ptr<ComponentTransferView> feView(new ComponentTransferView(fe, findInput(fe.input_)));
    if (!fe.result_.empty())
      namedFilters_[fe.result_] = feView;
    lastFilter_ = feView;
  }

  void operator()(feOffset const & fe)
  {
    boost::shared_ptr<OffsetView> feView(new OffsetView(fe, findInput(fe.input_)));
    if (!fe.result_.empty())
      namedFilters_[fe.result_] = feView;
    lastFilter_ = feView;
  }

  void operator()(feComposite const & fe)
  {
    boost::shared_ptr<CompositeView> feView(new CompositeView(fe, findInput(fe.input1_), findInput(fe.input2_)));
    if (!fe.result_.empty())
      namedFilters_[fe.result_] = feView;
    lastFilter_ = feView;
  }

  void operator()(feMerge const & fe)
  {
    boost::shared_ptr<MergeView> feView(new MergeView);
    for(std::vector<FilterInput>::const_iterator f = fe.inputs_.begin();
      f != fe.inputs_.end(); ++f)
      feView->addNode(findInput(*f));
    if (!fe.result_.empty())
      namedFilters_[fe.result_] = feView;
    lastFilter_ = feView;
  }

  void operator()(feColorMatrix const & fe)
  {
    boost::shared_ptr<ColorMatrixView> feView(new ColorMatrixView(fe, findInput(fe.input_)));
    if (!fe.result_.empty())
      namedFilters_[fe.result_] = feView;
    lastFilter_ = feView;
  }

  IFilterViewPtr const & lastFilter() { return lastFilter_; }

private:
  Filters::Input const & input_;
  typedef std::map<svg_string_t, IFilterViewPtr> NamedFilters;
  NamedFilters namedFilters_;
  IFilterViewPtr lastFilter_;
  mutable IFilterViewPtr sourceAlphaView_, backgroundAlphaView_;

  IFilterViewPtr findInput(FilterInput const & in) const
  {
    switch (in.source_)
    {
    case FilterInput::fiReference:
    {
      NamedFilters::const_iterator f = namedFilters_.find(in.reference_);
      if (f == namedFilters_.end())
        throw std::runtime_error("Can't find filter element");
      return f->second;
    }
    case FilterInput::fiNotSet:
      if (lastFilter_)
        return lastFilter_;
      else
        return input_.sourceGraphic_;
    case FilterInput::fiSourceGraphic:
      return input_.sourceGraphic_;
    case FilterInput::fiSourceAlpha:
      if (!sourceAlphaView_)
        sourceAlphaView_.reset(new AlphaChannelView(input_.sourceGraphic_));
      return sourceAlphaView_;
    case FilterInput::fiBackgroundImage:
      return input_.backgroundImage_;
    case FilterInput::fiBackgroundAlpha:
      if (!backgroundAlphaView_)
        backgroundAlphaView_.reset(new AlphaChannelView(input_.backgroundImage_));
      return backgroundAlphaView_;
    case FilterInput::fiFillPaint:
      return input_.fillPaint_;
    case FilterInput::fiStrokePaint:
      return input_.strokePaint_;
    default:
      BOOST_ASSERT(false);
    }
    return IFilterViewPtr();
  }
};

namespace
{
  length_factory_t length_factory_instance;

  struct length_policy_t
  {
    typedef length_factory_t const length_factory_type;

    static length_factory_type & length_factory(ElementWithRegionContext const &)
    {
      return length_factory_instance;
    }
  };
}

IFilterViewPtr Filters::get(svg_string_t const & id, length_factory_t const &, Input const & input)
{
  try
  {
    if (XMLElement node = xml_document_.findElementById(id))
    {
      FilterContext filterContext;
      svgpp::document_traversal<
        svgpp::context_factories<context_factories>,
        svgpp::color_factory<color_factory_t>,
        svgpp::length_policy<length_policy_t>,
        svgpp::processed_elements<
          boost::mpl::set<
            svgpp::tag::element::filter,
            svgpp::tag::element::feBlend,
            svgpp::tag::element::feMerge,
            svgpp::tag::element::feMergeNode,
            svgpp::tag::element::feDistantLight,
            svgpp::tag::element::fePointLight,
            svgpp::tag::element::feSpotLight,
            svgpp::tag::element::feColorMatrix,
            svgpp::tag::element::feComponentTransfer,
            svgpp::tag::element::feComposite,
            svgpp::tag::element::feFuncA,
            svgpp::tag::element::feFuncB,
            svgpp::tag::element::feFuncG,
            svgpp::tag::element::feFuncR,
            svgpp::tag::element::feOffset
          >::type
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
            boost::mpl::pair<svgpp::tag::element::feOffset, svgpp::tag::attribute::dx>,
            boost::mpl::pair<svgpp::tag::element::feOffset, svgpp::tag::attribute::dy>,
            boost::mpl::pair<svgpp::tag::element::feComposite, svgpp::tag::attribute::operator_>,
            boost::mpl::pair<svgpp::tag::element::feComposite, svgpp::tag::attribute::k1>,
            boost::mpl::pair<svgpp::tag::element::feComposite, svgpp::tag::attribute::k2>,
            boost::mpl::pair<svgpp::tag::element::feComposite, svgpp::tag::attribute::k3>,
            boost::mpl::pair<svgpp::tag::element::feComposite, svgpp::tag::attribute::k4>,

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
          >::type
        >
      >::load_expected_element(node, filterContext, svgpp::tag::element::filter());

      FilterElementVisitor v(input);
      for(std::vector<FilterElement>::const_iterator fe = filterContext.elements_.begin();
        fe != filterContext.elements_.end(); ++fe)
      {
        boost::apply_visitor(v, *fe);
      }
      if (!v.lastFilter())
        throw std::runtime_error("No filter elements in filter definition");
      return v.lastFilter();
    }
    else
      throw std::runtime_error("Filter not found");
  }
  catch (std::exception const & e)
  {
    // Not all filters implemented yet, we will skip such cases
    std::cerr << e.what() << "\n";
    return input.sourceGraphic_;
  }
}