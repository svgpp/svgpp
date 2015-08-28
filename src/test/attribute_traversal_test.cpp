#include <svgpp/attribute_traversal/attribute_traversal.hpp>
#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>

#include <gtest/gtest.h>

#define TEXT(x) #x

namespace
{
  struct after_viewport_attributes_tag {};
  struct before_deferred_attributes_tag {};

  class traversal_context
  {
  public:
    struct context_type {};

    context_type & context() 
    { 
      static context_type instance;
      return instance;
    }

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
    bool notify(after_viewport_attributes_tag)
    {
      log_ << "viewport_attributes_loaded\n";
      return true;
    }

    bool notify(before_deferred_attributes_tag)
    {
      log_ << "before_deferred_attributes\n";
      return true;
    }
  };

  char const xml1[] = 
    TEXT(<some_element onunload="call_onunload()" x="11" y="12" style="baseline-shift:style value;fill: fill value ;" baseline-shift="attr value" font-weight="fw value" />)
    ;
}

namespace
{
  struct traversal_policy: public svgpp::policy::attribute_traversal::default_policy
  {
    typedef boost::mpl::always<
      boost::mpl::joint_view<
          boost::mpl::joint_view<
            svgpp::traits::viewport_attributes, 
            boost::mpl::single_view<svgpp::notify_context<after_viewport_attributes_tag> > 
          >::type,
          svgpp::traits::font_selection_attributes
      >::type
    > get_priority_attributes_by_element;

    typedef boost::mpl::always<
      boost::mpl::joint_view<
        boost::mpl::single_view<svgpp::notify_context<before_deferred_attributes_tag> >,
        svgpp::traits::document_event_attributes
      >::type
    > get_deferred_attributes_by_element;
  };

  struct traversal_policy_wo_style: traversal_policy
  {
    static const bool parse_style = false;
  };

  struct traversal_policy_with_style: svgpp::policy::attribute_traversal::raw
  {
    static const bool parse_style = true;
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
    svgpp::attribute_traversal_policy<traversal_policy> 
  >::type::load(svg_element->first_attribute(), context)));
  {
    using namespace svgpp;
    using namespace svgpp::detail;
    traversal_context2 sample_context;
    sample_context.load_attribute(attribute_id_y, boost::as_literal("12"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_x, boost::as_literal("11"), tag::source::attribute());
    sample_context.notify(after_viewport_attributes_tag());
    sample_context.load_attribute(attribute_id_font_weight, boost::as_literal("fw value"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_baseline_shift, boost::as_literal("style value"), tag::source::css());
    sample_context.load_attribute(attribute_id_fill, boost::as_literal("fill value"), tag::source::css());
    sample_context.notify(before_deferred_attributes_tag());
    sample_context.load_attribute(attribute_id_onunload, boost::as_literal("call_onunload()"), tag::source::attribute());
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
      svgpp::attribute_traversal_policy<traversal_policy_wo_style> 
    >::type::load(svg_element->first_attribute(), context)));
  {
    using namespace svgpp;
    using namespace svgpp::detail;
    traversal_context2 sample_context;
    sample_context.load_attribute(attribute_id_y, boost::as_literal("12"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_x, boost::as_literal("11"), tag::source::attribute());
    sample_context.notify(after_viewport_attributes_tag());
    sample_context.load_attribute(attribute_id_font_weight, boost::as_literal("fw value"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_baseline_shift, boost::as_literal("attr value"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_style, boost::as_literal("baseline-shift:style value;fill: fill value ;"), tag::source::attribute());
    sample_context.notify(before_deferred_attributes_tag());
    sample_context.load_attribute(attribute_id_onunload, boost::as_literal("call_onunload()"), tag::source::attribute());
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
      svgpp::attribute_traversal_policy<traversal_policy_with_style> 
    >::type::load(svg_element->first_attribute(), context)));
  {
    using namespace svgpp;
    using namespace svgpp::detail;
    traversal_context sample_context;
    sample_context.load_attribute(attribute_id_onunload, boost::as_literal("call_onunload()"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_x, boost::as_literal("11"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_y, boost::as_literal("12"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_baseline_shift, boost::as_literal("style value"), tag::source::css());
    sample_context.load_attribute(attribute_id_fill, boost::as_literal("fill value"), tag::source::css());
    sample_context.load_attribute(attribute_id_baseline_shift, boost::as_literal("attr value"), tag::source::attribute());
    sample_context.load_attribute(attribute_id_font_weight, boost::as_literal("fw value"), tag::source::attribute());
    EXPECT_EQ(sample_context.str(), context.str());
  }
}