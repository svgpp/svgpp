#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_SET_SIZE 40

#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>
#include <svgpp/document_traversal.hpp>
#include <boost/mpl/set.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace mpl = boost::mpl;

class Filter;
class feComponentTransfer;

class ElementWithRegion
{
public:
  void set(svgpp::tag::attribute::x, double val)  { x_ = val; }
  void set(svgpp::tag::attribute::y, double val)  { y_ = val; }
  void set(svgpp::tag::attribute::width, double val) { width_ = val; }
  void set(svgpp::tag::attribute::height, double val) { height_ = val; }

protected:
  double x_, y_, width_, height_;
};

enum FilterInput { fiSourceGraphic, fiSourceAlpha, fiBackgroundImage, 
  fiBackgroundAlpha, fiFillPaint, fiStrokePaint, fiReference };

typedef mpl::map<
  mpl::pair< svgpp::tag::value::SourceGraphic,    mpl::integral_c<FilterInput, fiSourceGraphic> >,
  mpl::pair< svgpp::tag::value::SourceAlpha,      mpl::integral_c<FilterInput, fiSourceAlpha> >,
  mpl::pair< svgpp::tag::value::BackgroundImage,  mpl::integral_c<FilterInput, fiBackgroundImage> >,
  mpl::pair< svgpp::tag::value::BackgroundAlpha,  mpl::integral_c<FilterInput, fiBackgroundAlpha> >,
  mpl::pair< svgpp::tag::value::FillPaint,        mpl::integral_c<FilterInput, fiFillPaint> >,
  mpl::pair< svgpp::tag::value::StrokePaint,      mpl::integral_c<FilterInput, fiStrokePaint> >
> filter_input_to_enum;

template<class InAttributeTag>
class ElementWithIn
{
public:
  template<class Input>
  void set(InAttributeTag, Input)
  {
    filterInput_ = mpl::at<filter_input_to_enum, Input>::type::value;
  }

  void set(InAttributeTag, boost::iterator_range<const char *> const & reference)
  {
    filterInput_ = fiReference;
    reference_.assign(boost::begin(reference), boost::end(reference));
  }

protected:
  FilterInput filterInput_;
  std::string reference_;
};

class FilterElement: public ElementWithRegion
{
public:
  using ElementWithRegion::set;

  template<class Range>
  void set(svgpp::tag::attribute::result, Range const & r)
  {
    result_.assign(boost::begin(r), boost::end(r));
  }

protected:
  std::string result_;
};

class feBlend: 
  public FilterElement,
  public ElementWithIn<svgpp::tag::attribute::in>,
  public ElementWithIn<svgpp::tag::attribute::in2>
{
  enum BlendMode { mNormal, mMultiply, mScreen, mDarken, mLighten};

  typedef mpl::map<
    mpl::pair< svgpp::tag::value::normal,   mpl::integral_c<BlendMode, mNormal> >,
    mpl::pair< svgpp::tag::value::multiply, mpl::integral_c<BlendMode, mMultiply> >,
    mpl::pair< svgpp::tag::value::screen,   mpl::integral_c<BlendMode, mScreen> >,
    mpl::pair< svgpp::tag::value::darken,   mpl::integral_c<BlendMode, mDarken> >,
    mpl::pair< svgpp::tag::value::lighten,  mpl::integral_c<BlendMode, mLighten> >
  > mode_to_enum;

public:
  feBlend(Filter & parent)
    : parent_(parent)
  {
  }

  void on_exit_element();

  using FilterElement::set;
  using ElementWithIn<svgpp::tag::attribute::in>::set;
  using ElementWithIn<svgpp::tag::attribute::in2>::set;

  template<class Mode>
  void set(svgpp::tag::attribute::mode, Mode)
  {
    mode_ = mpl::at<mode_to_enum, Mode>::type::value;
  }

private:
  Filter & parent_;
  BlendMode mode_;
};

struct feFuncData
{
  double slope_, intercept_, amplitude_, exponent_, offset_;
  std::vector<double> tableValues_;
};

class feFunc
{
public:
  feFunc(feComponentTransfer & parent)
    : parent_(parent)
  {}

  template<class Type>
  void set(svgpp::tag::attribute::type, Type)
  {
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
  feComponentTransfer & parent_;
  feFuncData data_;
};

enum ARGBComponent { argbA, argbR, argbG, argbB };

class feComponentTransfer:
  public FilterElement,
  public ElementWithIn<svgpp::tag::attribute::in>
{
public:
  feComponentTransfer(Filter & parent)
    : parent_(parent)
  {
  }

  void on_exit_element() const {}

  void setComponentFunction(ARGBComponent component, feFuncData const & func)
  {
    func_[component] = func;
  }

  using FilterElement::set;
  using ElementWithIn<svgpp::tag::attribute::in>::set;

private:
  Filter & parent_;
  feFuncData func_[4];
};

template<ARGBComponent ComponentArg>
class feFuncX: public feFunc
{
public:
  static const ARGBComponent Component = ComponentArg;

  feFuncX(feComponentTransfer & parent)
    : feFunc(parent)
  {}

  void on_exit_element()
  {
    parent_.setComponentFunction(Component, data_);
  }
};

class Filter: public ElementWithRegion
{
public:
  Filter()
  {}

  void on_exit_element()
  {
  }

  using ElementWithRegion::set;

  template<class StringRange>
  void set(svgpp::tag::attribute::id, StringRange const & str)
  {
    id_.assign(boost::begin(str), boost::end(str));
  }

  void set(svgpp::tag::attribute::filterUnits, svgpp::tag::value::userSpaceOnUse)
  {
    filterUnitsUseObjectBoundingBox_ = false;
  }

  void set(svgpp::tag::attribute::filterUnits, svgpp::tag::value::objectBoundingBox)
  {
    filterUnitsUseObjectBoundingBox_ = true;
  }

private:
  std::string id_;
  bool filterUnitsUseObjectBoundingBox_;
};

void feBlend::on_exit_element()
{
}

class Canvas
{
public:
  void on_enter_element(svgpp::tag::element::svg const &) {}
  void on_exit_element() {}

  boost::shared_ptr<Filter> get_child_context(svgpp::tag::element::filter const &)
  {
    return boost::make_shared<Filter>();
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
struct ContextFactories::apply<ParentContext, svgpp::tag::element::filter>
{
  typedef svgpp::factory::context::get_ptr_from_parent<svgpp::tag::element::filter, boost::shared_ptr<Filter> > type;
};

template<class ParentContext>
struct ContextFactories::apply<ParentContext, svgpp::tag::element::feBlend>
{
  typedef svgpp::factory::context::on_stack<feBlend> type;
};

template<class ParentContext>
struct ContextFactories::apply<ParentContext, svgpp::tag::element::feComponentTransfer>
{
  typedef svgpp::factory::context::on_stack<feComponentTransfer> type;
};

template<>
struct ContextFactories::apply<feComponentTransfer, svgpp::tag::element::feFuncA>
{
  typedef svgpp::factory::context::on_stack<feFuncX<argbA> > type;
};

template<>
struct ContextFactories::apply<feComponentTransfer, svgpp::tag::element::feFuncR>
{
  typedef svgpp::factory::context::on_stack<feFuncX<argbR> > type;
};

template<>
struct ContextFactories::apply<feComponentTransfer, svgpp::tag::element::feFuncG>
{
  typedef svgpp::factory::context::on_stack<feFuncX<argbG> > type;
};

template<>
struct ContextFactories::apply<feComponentTransfer, svgpp::tag::element::feFuncB>
{
  typedef svgpp::factory::context::on_stack<feFuncX<argbB> > type;
};

int main()
{
  char text[] = "<svg/>";
  rapidxml_ns::xml_document<> doc;    // character type defaults to char
  doc.parse<0>(text);  
  if (rapidxml_ns::xml_node<> * svg_element = doc.first_node("svg"))
  {
    Canvas canvas;
    svgpp::document_traversal<
      svgpp::context_factories<ContextFactories>,
      svgpp::processed_elements<
        boost::mpl::set<
          svgpp::tag::element::svg,
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
        >::type
      >,
      svgpp::processed_attributes<
        boost::mpl::set<
          boost::mpl::pair<svgpp::tag::element::filter, svgpp::tag::attribute::id>,
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
        >::type
      >
    >::load_document(svg_element, canvas);
  }
  return 0;
}