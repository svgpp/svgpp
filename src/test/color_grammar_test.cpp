#include <svgpp/parser/grammar/color.hpp>
#include <svgpp/factory/integer_color.hpp>

#include <gtest/gtest.h>
#include <boost/tuple/tuple_io.hpp>

namespace qi = boost::spirit::qi;

namespace
{

struct color_factory_base
{
  typedef boost::tuple<unsigned char, unsigned char, unsigned char> color_type;

  static color_type create(unsigned char r, unsigned char g, unsigned char b)
  {
    return color_type(r, g, b);
  }
};

typedef svgpp::factory::color::percentage_adapter<color_factory_base> color_factory;

template<class StringT>
void valid_testT(StringT const & testStr, int r, int g, int b)
{
  typename StringT::const_iterator first = testStr.begin();
  typedef svgpp::color_grammar<
    svgpp::tag::source::attribute,
    typename StringT::const_iterator, color_factory
  > grammar_t;
  grammar_t grammar;
  typename grammar_t::color_type color;
  EXPECT_TRUE(qi::parse(first, testStr.end(), grammar, color));
  EXPECT_TRUE(first == testStr.end());
  EXPECT_EQ(r, color.template get<0>());
  EXPECT_EQ(g, color.template get<1>());
  EXPECT_EQ(b, color.template get<2>());
}

template<class StringT>
void invalid_testT(StringT const & testStr)
{
  typename StringT::const_iterator first = testStr.begin();
  svgpp::color_grammar<svgpp::tag::source::attribute, typename StringT::const_iterator, color_factory> grammar;
  EXPECT_TRUE(!qi::parse(first, testStr.end(), grammar) || first != testStr.end());
}

typedef boost::tuple<const char *, int, int, int> color_test_t;

color_test_t ValidColorTests[] = {
  color_test_t("rgb( 1, 2, 3 )", 1, 2, 3),
  color_test_t("rgb(1,2,3)", 1, 2, 3),
  color_test_t("rgb(111,222,132)", 111, 222, 132),
  color_test_t("rgb(111 ,222 , 132)", 111, 222, 132),
  color_test_t("rgb( 0%, 50%, 100%)", 0, 127, 255),
  color_test_t("rgb(66.667%,0%,6.667%)", 170, 0, 17),
  color_test_t("#ffffff", 0xff, 0xff, 0xff),
  color_test_t("#aBCdeF", 0xab, 0xcd, 0xef),
  color_test_t("#123456", 0x12, 0x34, 0x56),
  color_test_t("#123", 0x11, 0x22, 0x33),
  color_test_t("#000000", 0, 0, 0),
  color_test_t("aliceblue", 240, 248, 255),
  color_test_t("blanchedalmond", 255, 235, 205),
  color_test_t("yellowgreen", 154, 205, 50),
  color_test_t("YeLlOwGrEeN", 154, 205, 50)
};

std::ostream & operator<< (std::ostream & os, color_test_t const & val)
{
  return boost::tuples::operator<<(os, val);
}

class ValidColor : public ::testing::TestWithParam<color_test_t> {
};

char const * const InvalidColorStrings[] = {
  "rgb(111,,222,132)",
  "rgb(111,,132)",
  "rgb (111,222,132)",
  " rgb(1,2,3)",
  "rgb 1,2,3",
  "rgb(1,2,3) ",
  "rgb(1,2,3,)",
  "rgb(1,256,3)",
  "rgb(10%,12,13)",
  "rgb(10%,12%,13)",
  "rgb(10%,12%,13 %)",
  "#ab",
  "#abcd",
  "#abcde",
  "#abcdef0",
  "#abcdef ",
  " #abcdef",
  "# abcdef",
  "# abcde",
  "abcdef",
  " abcdef",
  "aliceblue ",
  " aliceblue",
  "aliceblud",
  "aliceblu",
};

}

TEST_P(ValidColor, Valid)
{
  std::string testStr = GetParam().get<0>();
  valid_testT(testStr, 
    GetParam().get<1>(), GetParam().get<2>(), GetParam().get<3>());
  valid_testT(std::wstring(testStr.begin(), testStr.end()), 
    GetParam().get<1>(), GetParam().get<2>(), GetParam().get<3>());
}

INSTANTIATE_TEST_CASE_P(color_grammar,
                        ValidColor,
                        ::testing::ValuesIn(ValidColorTests));


class InvalidColor : public ::testing::TestWithParam<const char*> {
};

TEST_P(InvalidColor, Invalid)
{
  std::string testStr = GetParam();
  invalid_testT(testStr);
  invalid_testT(std::wstring(testStr.begin(), testStr.end()) );
}

INSTANTIATE_TEST_CASE_P(color_grammar,
                        InvalidColor,
                        ::testing::ValuesIn(InvalidColorStrings));
