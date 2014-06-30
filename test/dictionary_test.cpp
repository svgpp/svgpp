#include <svgpp/detail/names_dictionary.hpp>

#include <gtest/gtest.h>

#define SVGPP_ON_NS(ns, name, str)

TEST(Dictionary, ElementChar)
{
  typedef svgpp::detail::element_name_to_id_dictionary dict;
  EXPECT_EQ(dict::find("font-face-uri"), svgpp::detail::element_type_id_font_face_uri);
  EXPECT_EQ(dict::find("font-face-ur"), svgpp::detail::unknown_element_type_id);
  EXPECT_EQ(dict::find("font-face-uri "), svgpp::detail::unknown_element_type_id);
  EXPECT_EQ(dict::find("font-face-urj"), svgpp::detail::unknown_element_type_id);
  EXPECT_EQ(dict::find("a"), svgpp::detail::element_type_id_a);
  EXPECT_EQ(dict::find("0"), svgpp::detail::unknown_element_type_id);
  EXPECT_EQ(dict::find("aaaa"), svgpp::detail::unknown_element_type_id);
  EXPECT_EQ(dict::find("vkern"), svgpp::detail::element_type_id_vkern);
  EXPECT_EQ(dict::find("vkern "), svgpp::detail::unknown_element_type_id);
  EXPECT_EQ(dict::find("vkero"), svgpp::detail::unknown_element_type_id);

#define SVGPP_ON(name, str) EXPECT_EQ(svgpp::detail::element_type_id_ ## name, dict::find(#str));
#include <svgpp/detail/dict/enumerate_all_elements.inc>
#undef SVGPP_ON
}

TEST(Dictionary, ElementWChar)
{
  typedef svgpp::detail::element_name_to_id_dictionary dict;
  EXPECT_EQ(dict::find(L"font-face-uri"), svgpp::detail::element_type_id_font_face_uri);
  EXPECT_EQ(dict::find(std::wstring(L"font-face-uri")), svgpp::detail::element_type_id_font_face_uri);

#define SVGPP_ON(name, str) EXPECT_EQ(svgpp::detail::element_type_id_ ## name, dict::find(L ## #str));
#include <svgpp/detail/dict/enumerate_all_elements.inc>
#undef SVGPP_ON
}

TEST(Dictionary, AttributeChar)
{
  typedef svgpp::detail::svg_attribute_name_to_id_dictionary dict;
  EXPECT_EQ(dict::find("baseline-shift"), svgpp::detail::attribute_id_baseline_shift);
  EXPECT_EQ(dict::find("baseline-shifT"), svgpp::detail::unknown_attribute_id);
  EXPECT_EQ(dict::find("specularConstant"), svgpp::detail::attribute_id_specularConstant);
  EXPECT_EQ(dict::find("specularconstant"), svgpp::detail::unknown_attribute_id);
#define SVGPP_ON(name, str) EXPECT_EQ(svgpp::detail::attribute_id_ ## name, dict::find(#str));
#define SVGPP_ON_STYLE(name, str) SVGPP_ON(name, str)
#include <svgpp/detail/dict/enumerate_all_attributes.inc>
#undef SVGPP_ON
#undef SVGPP_ON_STYLE
}

TEST(Dictionary, CSSProperty)
{
  typedef svgpp::detail::css_property_name_to_id_dictionary dict;
  EXPECT_EQ(dict::find_ignore_case("baseline-shift"), svgpp::detail::attribute_id_baseline_shift);
  EXPECT_EQ(dict::find_ignore_case("baseline-shifT"), svgpp::detail::attribute_id_baseline_shift);
  EXPECT_EQ(dict::find_ignore_case(std::wstring(L"leTtEr-spacIng")), svgpp::detail::attribute_id_letter_spacing);

#define SVGPP_ON(name, str) EXPECT_EQ(svgpp::detail::unknown_attribute_id, dict::find(#str));
#define SVGPP_ON_STYLE(name, str) EXPECT_EQ(svgpp::detail::attribute_id_ ## name, dict::find(#str));
#include <svgpp/detail/dict/enumerate_all_attributes.inc>
#undef SVGPP_ON
#undef SVGPP_ON_STYLE

#define SVGPP_ON(name, str) 
#define SVGPP_ON_STYLE(name, str) EXPECT_EQ(boost::to_lower_copy(std::string(#str)), #str);
#include <svgpp/detail/dict/enumerate_all_attributes.inc>
#undef SVGPP_ON
#undef SVGPP_ON_STYLE
}