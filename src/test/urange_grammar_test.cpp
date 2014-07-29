#include <svgpp/parser/grammar/urange.hpp>

#include <gtest/gtest.h>
#include <boost/tuple/tuple_io.hpp>

namespace qi = boost::spirit::qi;

namespace
{

template<class StringT>
void valid_testT(StringT const & testStr, int r1, int r2)
{
  typename StringT::const_iterator first = testStr.begin();
  svgpp::urange_grammar<typename StringT::const_iterator> grammar;
  std::pair<unsigned, unsigned> urange;
  EXPECT_TRUE(qi::parse(first, testStr.end(), grammar, urange));
  EXPECT_TRUE(first == testStr.end());
  EXPECT_EQ(r1, urange.first);
  EXPECT_EQ(r2, urange.second);
}

template<class StringT>
void invalid_testT(StringT const & testStr)
{
  typename StringT::const_iterator first = testStr.begin();
  svgpp::urange_grammar<typename StringT::const_iterator> grammar;
  EXPECT_TRUE(!qi::parse(first, testStr.end(), grammar) || first != testStr.end());
}

typedef boost::tuple<const char *, int, int> urange_test_t;

urange_test_t ValidTests[] = {
  urange_test_t("U+0-7FFFFFFF", 0, 0x7FFFFFFF),
  urange_test_t("U+05D1", 0x05D1, 0x05D1),
  urange_test_t("U+20A7", 0x20A7, 0x20A7),
  urange_test_t("U+215?", 0x2150, 0x215F),
  urange_test_t("U+00??", 0x0, 0xFF),
  urange_test_t("U+E??", 0xE00, 0xEFF),
  urange_test_t("U+AC00-D7FF", 0xAC00, 0xD7FF),
  urange_test_t("U+370-3FF", 0x370, 0x3FF),
  urange_test_t("U+3000-33FF", 0x3000, 0x33FF)
};

std::ostream & operator<< (std::ostream & os, urange_test_t const & val)
{
  return boost::tuples::operator<<(os, val);
}

class ValidURange : public ::testing::TestWithParam<urange_test_t> {
};

char const * const InvalidStrings[] = {
  "U+-7FF",
  " U+05D1",
  "U+05D1 ",
  "U+ 05D1",
  "U+00??A",
  "U+0- 7FFFFFFF"
};

}

TEST_P(ValidURange, Valid)
{
  std::string testStr = GetParam().get<0>();
  valid_testT(testStr, 
    GetParam().get<1>(), GetParam().get<2>());
  valid_testT(std::wstring(testStr.begin(), testStr.end()), 
    GetParam().get<1>(), GetParam().get<2>());
}

INSTANTIATE_TEST_CASE_P(urange_grammar,
                        ValidURange,
                        ::testing::ValuesIn(ValidTests));


class InvalidURange : public ::testing::TestWithParam<const char*> {
};

TEST_P(InvalidURange, Invalid)
{
  std::string testStr = GetParam();
  invalid_testT(testStr);
  invalid_testT(std::wstring(testStr.begin(), testStr.end()) );
}

INSTANTIATE_TEST_CASE_P(urange_grammar,
                        InvalidURange,
                        ::testing::ValuesIn(InvalidStrings));
