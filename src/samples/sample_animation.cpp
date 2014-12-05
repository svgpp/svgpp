#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_SET_SIZE 30

#include <rapidxml_ns/rapidxml_ns_utils.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>
#include <svgpp/svgpp.hpp>

using namespace svgpp;

struct NotAnimateElementContext
{
  void on_enter_element(tag::element::any const &) {}
  void on_exit_element() {}

  template<class T>
  static void set_text(T const & value) {}
};

struct BaseContext
{
  void set(tag::attribute::attributeType, tag::value::CSS) {}
  void set(tag::attribute::attributeType, tag::value::XML) {}
  void set(tag::attribute::attributeType, tag::value::auto_) {}

  template<class StringRange>
  void set(tag::attribute::attributeName, StringRange const & range) 
  {
    std::string name(boost::begin(range), boost::end(range));
  }

  void set(tag::attribute::dur, tag::value::media) {}
  void set(tag::attribute::dur, tag::value::indefinite) {}
  void set(tag::attribute::dur, double clock_seconds) {}

  void set(tag::attribute::repeatDur, tag::value::indefinite) {}
  void set(tag::attribute::repeatDur, double clock_seconds) {}

  void set(tag::attribute::repeatCount, tag::value::indefinite) {}
  void set(tag::attribute::repeatCount, double number) {}

  void set(tag::attribute::min, tag::value::media) {}
  void set(tag::attribute::min, double clock_seconds) {}
  void set(tag::attribute::max, tag::value::media) {}
  void set(tag::attribute::max, double clock_seconds) {}

  void set(tag::attribute::restart, tag::value::always) {}
  void set(tag::attribute::restart, tag::value::whenNotActive) {}
  void set(tag::attribute::restart, tag::value::never) {}

  void set(tag::attribute::fill, tag::value::freeze) {}
  void set(tag::attribute::fill, tag::value::remove) {}

  void set(tag::attribute::calcMode, tag::value::discrete) {}
  void set(tag::attribute::calcMode, tag::value::linear) {}
  void set(tag::attribute::calcMode, tag::value::paced) {}
  void set(tag::attribute::calcMode, tag::value::spline) {}

  void set(tag::attribute::additive, tag::value::replace) {}
  void set(tag::attribute::additive, tag::value::sum) {}

  void set(tag::attribute::accumulate, tag::value::none) {}
  void set(tag::attribute::accumulate, tag::value::sum) {}

  // Not parsed by SVG++
  template<class StringRange>
  void set(tag::attribute::begin, StringRange const &) {}
  template<class StringRange>
  void set(tag::attribute::keyTimes, StringRange const &) {}
  template<class StringRange>
  void set(tag::attribute::keySplines, StringRange const &) {}
  template<class StringRange>
  void set(tag::attribute::values, StringRange const &) {}
  template<class StringRange>
  void set(tag::attribute::from, StringRange const &) {}
  template<class StringRange>
  void set(tag::attribute::to, StringRange const &) {}
  template<class StringRange>
  void set(tag::attribute::by, StringRange const &) {}
  template<class StringRange>
  void set(tag::attribute::end, StringRange const &) {}
};

struct AnimateContext: BaseContext
{
  template<class XMLElement>
  AnimateContext(NotAnimateElementContext const & parent, XMLElement const & xml_element)
  {}

  void on_exit_element() {}
};

struct ContextFactories
{
  template<class ParentContext, class ElementTag>
  struct apply
  {
    typedef svgpp::factory::context::same<ParentContext, ElementTag> type;
  };
};

template<>
struct ContextFactories::apply<NotAnimateElementContext, svgpp::tag::element::animate>
{
  typedef svgpp::factory::context::on_stack_with_xml_element<AnimateContext> type;
};

template<>
struct ContextFactories::apply<NotAnimateElementContext, svgpp::tag::element::animateColor>
{
  typedef svgpp::factory::context::on_stack_with_xml_element<AnimateContext> type;
};

template<>
struct ContextFactories::apply<NotAnimateElementContext, svgpp::tag::element::animateMotion>
{
  typedef svgpp::factory::context::on_stack_with_xml_element<AnimateContext> type;
};

template<>
struct ContextFactories::apply<NotAnimateElementContext, svgpp::tag::element::animateTransform>
{
  typedef svgpp::factory::context::on_stack_with_xml_element<AnimateContext> type;
};

template<>
struct ContextFactories::apply<NotAnimateElementContext, svgpp::tag::element::set>
{
  typedef svgpp::factory::context::on_stack_with_xml_element<AnimateContext> type;
};

void parse(rapidxml_ns::xml_node<char> const * svg_element)
{
  NotAnimateElementContext context;
  document_traversal<
    context_factories<ContextFactories>,
    ignored_elements<
      boost::mpl::insert<
        traits::descriptive_elements,
        tag::element::mpath
      >::type
    >,
    processed_attributes<boost::mpl::set<
      tag::attribute::attributeType, 
      tag::attribute::attributeName,
      tag::attribute::begin, 
      tag::attribute::dur, 
      tag::attribute::end, 
      tag::attribute::min, 
      tag::attribute::max, 
      tag::attribute::restart, 
      tag::attribute::repeatCount, 
      tag::attribute::repeatDur, 
      boost::mpl::pair<tag::element::animate, tag::attribute::fill>,
      tag::attribute::calcMode, 
      tag::attribute::keyTimes, 
      tag::attribute::keySplines, 
      tag::attribute::from, 
      tag::attribute::to, 
      tag::attribute::by,
      tag::attribute::additive, 
      tag::attribute::accumulate,
      boost::mpl::pair<tag::element::animate, tag::attribute::values>,
      boost::mpl::pair<tag::element::animateColor, tag::attribute::values>,
      boost::mpl::pair<tag::element::animateMotion, tag::attribute::values>,
      boost::mpl::pair<tag::element::animateTransform, tag::attribute::values>
    >::type>,
    passthrough_attributes<boost::mpl::set<
      tag::attribute::keyTimes, 
      tag::attribute::keySplines, 
      tag::attribute::values
    >::type>,
    basic_shapes_policy<policy::basic_shapes::raw>
  >::load_document(svg_element, context);
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
    parse(doc.first_node());
  }
  catch (std::exception const & e)
  {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}