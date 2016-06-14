#include <svgpp/document_traversal.hpp>
#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>
#include <sstream>

#include <gtest/gtest.h>

#define TEXT(x) #x

namespace
{
  struct AInSvgContext;

  struct SvgContext
  {
    SvgContext(std::ostringstream & log): log(log)   { log << "<svg>"; }
    SvgContext(SvgContext & parent): log(parent.log) { log << "<svg>"; }
    SvgContext(AInSvgContext   & parent);

    void on_exit_element() {}

    std::ostringstream & log;
  };

  struct AInSvgContext
  {
    AInSvgContext(SvgContext & parent) : log(parent.log) { log << "<aInSvg>"; }
    void on_exit_element() {}

    std::ostringstream & log;
  };

  struct RectContext
  {
    RectContext(SvgContext    & parent) { parent.log << "<rect>"; }
    RectContext(AInSvgContext & parent) { parent.log << "<rect>"; }
    void on_exit_element() {}
  };

  struct EllipseContext
  {
    EllipseContext(SvgContext    & parent) { parent.log << "<ellipse>"; }
    EllipseContext(AInSvgContext & parent)  { parent.log << "<rect>"; }
    void on_exit_element() {}
  };

  struct TextContext
  {
    TextContext(SvgContext    & parent): log(parent.log) { parent.log << "<text>"; }
    TextContext(AInSvgContext & parent): log(parent.log) { parent.log << "<text>"; }

    void on_exit_element() {}
    template<class TextRange>
    void set_text(TextRange const & text) { log << std::string(boost::begin(text), boost::end(text));}

    std::ostringstream & log;
  };

  struct AInTextContext
  {
    AInTextContext(TextContext & parent): log(parent.log) { parent.log << "<aInText>"; }

    void on_exit_element() {}
    template<class TextRange>
    void set_text(TextRange const & text) { log << std::string(boost::begin(text), boost::end(text)); }

    std::ostringstream & log;
  };

  SvgContext::SvgContext(AInSvgContext   & parent): log(parent.log) { log << "<svg>"; }

  struct context_factories
  {
    template<class ParentContext, class ElementTag>
    struct apply;
  };

  template<>
  struct context_factories::apply<SvgContext, svgpp::tag::element::svg>
  {
    typedef svgpp::factory::context::on_stack<SvgContext> type;
  };

  template<>
  struct context_factories::apply<SvgContext, svgpp::tag::element::rect>
  {
    typedef svgpp::factory::context::on_stack<RectContext> type;
  };

  template<>
  struct context_factories::apply<SvgContext, svgpp::tag::element::ellipse>
  {
    typedef svgpp::factory::context::on_stack<EllipseContext> type;
  };

  template<>
  struct context_factories::apply<SvgContext, svgpp::tag::element::text>
  {
    typedef svgpp::factory::context::on_stack<TextContext> type;
  };

  template<>
  struct context_factories::apply<SvgContext, svgpp::tag::element::a>
  {
    typedef svgpp::factory::context::on_stack<AInSvgContext> type;
  };

  template<>
  struct context_factories::apply<AInSvgContext, svgpp::tag::element::svg>
  {
    typedef svgpp::factory::context::on_stack<SvgContext> type;
  };

  template<>
  struct context_factories::apply<AInSvgContext, svgpp::tag::element::rect>
  {
    typedef svgpp::factory::context::on_stack<RectContext> type;
  };

  template<>
  struct context_factories::apply<AInSvgContext, svgpp::tag::element::ellipse>
  {
    typedef svgpp::factory::context::on_stack<EllipseContext> type;
  };

  template<>
  struct context_factories::apply<AInSvgContext, svgpp::tag::element::text>
  {
    typedef svgpp::factory::context::on_stack<TextContext> type;
  };

  template<>
  struct context_factories::apply<TextContext, svgpp::tag::element::a>
  {
    typedef svgpp::factory::context::on_stack<AInTextContext> type;
  };

  char const xml1[] =
    TEXT(<svg xmlns = "http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">)
    TEXT(<rect x=".01" y=".01" width="4.98" height="2.98" fill="none" stroke="blue" stroke-width=".03"/>)
    TEXT(<a xlink:href="http://www.w3.org">)
    TEXT(  <ellipse cx="2.5" cy="1.5" rx="2" ry="1" fill="red"/>)
    TEXT(</a>)
    TEXT(<text x="1" y="2">)
    TEXT(  <a xlink:href="http://www.w3.org">)
    TEXT(    Some text string)
    TEXT(  </a>)
    TEXT(</text>)
    TEXT(</svg>)
    ;
}

TEST(DocumentTraversal, a)
{
  std::vector<char> modified_xml(xml1, xml1 + strlen(xml1) + 1);
  rapidxml_ns::xml_document<char> doc;
  doc.parse<0>(&modified_xml[0]);  
  rapidxml_ns::xml_node<char> const * svg_element = doc.first_node();
  ASSERT_TRUE(svg_element != NULL);
  std::ostringstream str;
  SvgContext context(str);
  EXPECT_TRUE((
    svgpp::document_traversal<
      svgpp::processed_elements<
        boost::mpl::set<
          svgpp::tag::element::svg, 
          svgpp::tag::element::ellipse,
          svgpp::tag::element::rect,
          svgpp::tag::element::text,
          svgpp::tag::element::a
        >::type
      >,
      svgpp::processed_attributes<
        boost::mpl::set<>::type
      >,
      svgpp::context_factories<context_factories>,
      svgpp::basic_shapes_policy<svgpp::policy::basic_shapes::raw>
    >::load_document(svg_element, context)));

  std::ostringstream expected_str;
  {
    SvgContext c1(expected_str);
    RectContext c2(c1);
    AInSvgContext c3(c1);
    EllipseContext c4(c3);
    TextContext c5(c1);
    AInTextContext c6(c5);
    c6.set_text(std::string("Some text string"));
  }
  EXPECT_EQ(expected_str.str(), str.str());
}