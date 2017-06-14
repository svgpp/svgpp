#include <svgpp/document_traversal.hpp>
#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test_path_context.hpp"

#define TEXT(x) #x

namespace mpl = boost::mpl;

namespace 
{
  class Context
  {
  public:
    virtual void set_rect(double x, double y, double width, double height,
      double rx, double ry) = 0;
    virtual void set_line(double x1, double y1, double x2, double y2) = 0;
    virtual void set_circle(double cx, double cy, double r) = 0;
    virtual void set_ellipse(double cx, double cy, double rx, double ry) = 0;

    void on_enter_element(svgpp::tag::element::any) const {}
    void on_exit_element() const {}
  };

  class MockContext: public Context
  {
  public:
    MOCK_METHOD6(set_rect, void(double x, double y, double width, double height,
      double rx, double ry));
    MOCK_METHOD4(set_line, void(double x1, double y1, double x2, double y2));
    MOCK_METHOD3(set_circle, void(double cx, double cy, double r));
    MOCK_METHOD4(set_ellipse, void(double cx, double cy, double rx, double ry));
  };

  class SimpleRectContext: public test_path_context
  {
  public:
    virtual void set_rect(double x, double y, double width, double height) = 0;
    virtual void set_line(double x1, double y1, double x2, double y2) = 0;
    virtual void set_circle(double cx, double cy, double r) = 0;
    virtual void set_ellipse(double cx, double cy, double rx, double ry) = 0;

    void on_enter_element(svgpp::tag::element::any) const {}
    void on_exit_element() const {}
  };

  class MockSimpleRectContext: public SimpleRectContext
  {
  public:
    MOCK_METHOD4(set_rect, void(double x, double y, double width, double height));
    MOCK_METHOD4(set_line, void(double x1, double y1, double x2, double y2));
    MOCK_METHOD3(set_circle, void(double cx, double cy, double r));
    MOCK_METHOD4(set_ellipse, void(double cx, double cy, double rx, double ry));
  };

  class RawAttributesContext
  {
  public:
    template<class AttributeTag, class Value>
    void set(AttributeTag, Value) {}

    void on_enter_element(svgpp::tag::element::any) const {}
    void on_exit_element() const {}
  };

  class ConvertToPathContext: public test_path_context
  {
  public:
    void on_enter_element(svgpp::tag::element::any) const {}
    void on_exit_element() const {}
  };

  struct collect_attributes_basic_shapes_policy: svgpp::policy::basic_shapes::raw
  {
    typedef mpl::set<
      svgpp::tag::element::ellipse,
      svgpp::tag::element::rect,
      svgpp::tag::element::line,
      svgpp::tag::element::circle> collect_attributes;
  };

  struct collect_attributes_simple_rect_basic_shapes_policy: 
    svgpp::policy::basic_shapes::raw
  {
    typedef mpl::set<svgpp::tag::element::rect> convert_to_path;

    typedef mpl::set<
      svgpp::tag::element::ellipse,
      svgpp::tag::element::line,
      svgpp::tag::element::circle> collect_attributes;

    static const bool convert_only_rounded_rect_to_path = true;
  };

  char const xml1[] =
    TEXT(<svg xmlns = "http://www.w3.org/2000/svg">)
    TEXT(<rect x=".01" y=".5" width="4.98" height="2.98"/>)
    TEXT(<rect x="100" y="150" width="400" height="200" rx="0" ry="0"/>)
    TEXT(<rect x="100" y="150" width="400" height="200" rx="50"/>)
    TEXT(<rect x="100" y="150" width="400" height="200" ry="90"/>)
    TEXT(<rect x="100" y="150" width="400" height="200" rx="70" ry="80"/>)
    TEXT(<ellipse cx="2.5" cy="1.5" rx="2" ry="1"/>)
    TEXT(<line x1="100" y1="300" x2="350" y2="150"/>)
    TEXT(<circle cx="600" cy="200" r="100"/>)
    TEXT(</svg>)
    ;

  class BasicShapesTest : public ::testing::Test {
  protected:
    virtual void SetUp() {
      modified_xml_.assign(xml1, xml1 + strlen(xml1) + 1);
      doc_.parse<0>(&modified_xml_[0]);  
      ASSERT_TRUE(GetSvgElement() != NULL);
    }

    rapidxml_ns::xml_node<char> const * GetSvgElement() const
    { return doc_.first_node(); }

  private:
    std::vector<char> modified_xml_;
    rapidxml_ns::xml_document<char> doc_;
  };
}

TEST_F(BasicShapesTest, CollectAttributes)
{
  MockContext context;
  {
    ::testing::InSequence seq;
    EXPECT_CALL(context, set_rect(.01, .5, 4.98, 2.98, 0, 0));
    EXPECT_CALL(context, set_rect(100, 150, 400, 200, 0, 0));
    EXPECT_CALL(context, set_rect(100, 150, 400, 200, 50, 50));
    EXPECT_CALL(context, set_rect(100, 150, 400, 200, 90, 90));
    EXPECT_CALL(context, set_rect(100, 150, 400, 200, 70, 80));
    EXPECT_CALL(context, set_ellipse(2.5, 1.5, 2, 1));
    EXPECT_CALL(context, set_line(100, 300, 350, 150));
    EXPECT_CALL(context, set_circle(600, 200, 100));
  }
  EXPECT_TRUE((
    svgpp::document_traversal<
      svgpp::processed_elements<
        boost::mpl::set<
          svgpp::tag::element::svg, 
          svgpp::tag::element::ellipse,
          svgpp::tag::element::rect,
          svgpp::tag::element::line,
          svgpp::tag::element::circle
        >::type
      >,
      svgpp::processed_attributes<
        svgpp::traits::shapes_attributes_by_element
      >,
      svgpp::basic_shapes_policy<collect_attributes_basic_shapes_policy>
    >::load_document(GetSvgElement(), context)));
}

TEST_F(BasicShapesTest, CollectAttributesSimpleRect)
{
  MockSimpleRectContext context;
  {
    ::testing::InSequence seq;
    EXPECT_CALL(context, set_rect(.01, .5, 4.98, 2.98));
    EXPECT_CALL(context, set_rect(100, 150, 400, 200));
    EXPECT_CALL(context, set_ellipse(2.5, 1.5, 2, 1));
    EXPECT_CALL(context, set_line(100, 300, 350, 150));
    EXPECT_CALL(context, set_circle(600, 200, 100));
  }
  EXPECT_TRUE((
    svgpp::document_traversal<
      svgpp::processed_elements<
        boost::mpl::set<
          svgpp::tag::element::svg, 
          svgpp::tag::element::ellipse,
          svgpp::tag::element::rect,
          svgpp::tag::element::line,
          svgpp::tag::element::circle
        >::type
      >,
      svgpp::processed_attributes<
        svgpp::traits::shapes_attributes_by_element
      >,
      svgpp::basic_shapes_policy<collect_attributes_simple_rect_basic_shapes_policy>
    >::load_document(GetSvgElement(), context)));
}

TEST_F(BasicShapesTest, RawAttributes)
{
  RawAttributesContext context;
  EXPECT_TRUE((
    svgpp::document_traversal<
      svgpp::processed_elements<
        boost::mpl::set<
          svgpp::tag::element::svg, 
          svgpp::tag::element::ellipse,
          svgpp::tag::element::rect,
          svgpp::tag::element::line,
          svgpp::tag::element::circle
        >::type
      >,
      svgpp::processed_attributes<
        svgpp::traits::shapes_attributes_by_element
      >,
      svgpp::basic_shapes_policy<svgpp::policy::basic_shapes::raw>
    >::load_document(GetSvgElement(), context)));
}

TEST_F(BasicShapesTest, ConvertToPath)
{
  ConvertToPathContext context;
  EXPECT_TRUE((
    svgpp::document_traversal<
      svgpp::processed_elements<
        boost::mpl::set<
          svgpp::tag::element::svg, 
          svgpp::tag::element::ellipse,
          svgpp::tag::element::rect,
          svgpp::tag::element::line,
          svgpp::tag::element::circle
        >::type
      >,
      svgpp::processed_attributes<
        svgpp::traits::shapes_attributes_by_element
      >,
      svgpp::basic_shapes_policy<svgpp::policy::basic_shapes::all_to_path>
    >::load_document(GetSvgElement(), context)));
}