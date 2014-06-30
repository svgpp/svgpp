#include <svgpp/attribute_traversal.hpp>
#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/xml/rapidxml_ns.hpp>

#include <gtest/gtest.h>

#define TEXT(x) #x

namespace
{
  class traversal_context
  {
  public:
    bool load_attribute(svgpp::detail::attribute_id id, boost::iterator_range<char const *> const & value, 
      svgpp::tag::source::css)
    {
      log_ << "STYLE " << id << " [" << std::string(value.begin(), value.end()) << "]\n";
      return true;
    }

    bool load_attribute(svgpp::detail::attribute_id id, boost::iterator_range<char const *> const & value, 
      svgpp::tag::source::attribute)
    {
      log_ << "ATTR " << id << " [" << std::string(value.begin(), value.end()) << "]\n";
      return true;
    }

    std::string str() const 
    {
      return log_.str();
    }

  protected:
    std::ostringstream log_;
  };

  class traversal_context2: public traversal_context
  {
  public:
    void on_viewport_attributes_loaded()
    {
      log_ << "viewport_attributes_loaded\n";
    }
  };

  char const xml1[] = 
    TEXT(<some_element x="11" y="12" style="baseline-shift:style value;fill: fill value ;" baseline-shift="attr value" font-weight="fw value" />)
    ;
}

namespace
{
  struct after_viewport_attributes_op_tag
  {
    template<class Context>
    static bool apply(Context & c)
    {
      c.on_viewport_attributes_loaded();
      return true;
    }
  };

  struct traversal_policy: public svgpp::policy::attribute_traversal::default_policy
  {
    typedef boost::mpl::always<
      boost::mpl::joint_view<
          typename boost::mpl::joint_view<
            svgpp::traits::viewport_attributes, 
            boost::mpl::single_view<svgpp::context_operation<after_viewport_attributes_op_tag> > 
          >::type,
          svgpp::traits::font_selection_attributes
      >::type
    > get_priority_attributes_by_element;
  };
}

TEST(AttributeTraversal, Prioritized)
{
  std::vector<char> modified_xml(xml1, xml1 + strlen(xml1) + 1);
  rapidxml_ns::xml_document<char> doc;
  doc.parse<0>(&modified_xml[0]);  
  rapidxml_ns::xml_node<char> const * svg_element = doc.first_node();
  ASSERT_TRUE(svg_element != NULL);
  traversal_context2 context;
  EXPECT_TRUE((svgpp::attribute_traversal<
    svgpp::tag::element::svg, 
    true, 
    svgpp::attribute_traversal_policy<traversal_policy> 
  >::type::load(svg_element->first_attribute(), context)));
  {
    using namespace svgpp;
    using namespace svgpp::detail;
    traversal_context2 sample_context;
    sample_context.load_attribute(attribute_id_y, boost::as_literal("12"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_x, boost::as_literal("11"), tag::source::attribute());
    sample_context.on_viewport_attributes_loaded();
    sample_context.load_attribute(attribute_id_font_weight, boost::as_literal("fw value"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_baseline_shift, boost::as_literal("style value"), tag::source::css());
    sample_context.load_attribute(attribute_id_fill, boost::as_literal("fill value"), tag::source::css());
    EXPECT_EQ(sample_context.str(), context.str());
  }
}

TEST(AttributeTraversal, Prioritized_Without_Style)
{
  std::vector<char> modified_xml(xml1, xml1 + strlen(xml1) + 1);
  rapidxml_ns::xml_document<char> doc;
  doc.parse<0>(&modified_xml[0]);  
  rapidxml_ns::xml_node<char> const * svg_element = doc.first_node();
  ASSERT_TRUE(svg_element != NULL);
  traversal_context2 context;
  EXPECT_TRUE((svgpp::attribute_traversal<
      svgpp::tag::element::svg, 
      false, 
      svgpp::attribute_traversal_policy<traversal_policy> 
    >::type::load(svg_element->first_attribute(), context)));
  {
    using namespace svgpp;
    using namespace svgpp::detail;
    traversal_context2 sample_context;
    sample_context.load_attribute(attribute_id_y, boost::as_literal("12"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_x, boost::as_literal("11"), tag::source::attribute());
    sample_context.on_viewport_attributes_loaded();
    sample_context.load_attribute(attribute_id_font_weight, boost::as_literal("fw value"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_baseline_shift, boost::as_literal("attr value"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_style, boost::as_literal("baseline-shift:style value;fill: fill value ;"), tag::source::attribute());
    EXPECT_EQ(sample_context.str(), context.str());
  }
}

TEST(AttributeTraversal, Sequential)
{
  std::vector<char> modified_xml(xml1, xml1 + strlen(xml1) + 1);
  rapidxml_ns::xml_document<char> doc;
  doc.parse<0>(&modified_xml[0]);  
  rapidxml_ns::xml_node<char> const * svg_element = doc.first_node();
  ASSERT_TRUE(svg_element != NULL);
  traversal_context context;
  EXPECT_TRUE((svgpp::attribute_traversal<
      svgpp::tag::element::svg, 
      true, 
      svgpp::attribute_traversal_policy<svgpp::policy::attribute_traversal::raw> 
    >::type::load(svg_element->first_attribute(), context)));
  {
    using namespace svgpp;
    using namespace svgpp::detail;
    traversal_context sample_context;
    sample_context.load_attribute(attribute_id_x, boost::as_literal("11"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_y, boost::as_literal("12"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_baseline_shift, boost::as_literal("style value"), tag::source::css());
    sample_context.load_attribute(attribute_id_fill, boost::as_literal("fill value"), tag::source::css());
    sample_context.load_attribute(attribute_id_baseline_shift, boost::as_literal("attr value"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_font_weight, boost::as_literal("fw value"), tag::source::attribute());
    EXPECT_EQ(sample_context.str(), context.str());
  }
}