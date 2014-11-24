#include <svgpp/parser/grammar/clock_value.hpp>

#include <gtest/gtest.h>
#include <boost/tuple/tuple_io.hpp>

namespace qi = boost::spirit::qi;

namespace
{

template<class StringT>
void valid_testT(StringT const & testStr, double seconds)
{
  typename StringT::const_iterator first = testStr.begin();
  svgpp::clock_value_grammar<typename StringT::const_iterator> grammar;
  double value;
  EXPECT_TRUE(qi::parse(first, testStr.end(), grammar, value));
  EXPECT_TRUE(first == testStr.end());
  EXPECT_NEAR(seconds, value, seconds * 1e-7);
}

template<class StringT>
void invalid_testT(StringT const & testStr)
{
  typename StringT::const_iterator first = testStr.begin();
  svgpp::clock_value_grammar<typename StringT::const_iterator> grammar;
  EXPECT_TRUE(!qi::parse(first, testStr.end(), grammar) || first != testStr.end());
}

typedef boost::tuple<const char *, double> valid_case_t;

valid_case_t ValidTests[] = {
  valid_case_t("02:30:03", (2 * 60 + 30) * 60 + 3),
  valid_case_t("02:59:59", (2 * 60 + 59) * 60 + 59),
  valid_case_t("50:00:10.25", 50 * 3600 + 10.25),
  valid_case_t("02:33", 153),
  valid_case_t("00:10.5", 10.5),
  valid_case_t("3.2h", (3 * 60 + 12) * 60),
  valid_case_t("45min", 45 * 60),
  valid_case_t("30s", 30),
  valid_case_t("5ms", 0.005),
  valid_case_t("12.467", 12.467),
  valid_case_t("00.5s", 0.5),
  valid_case_t("00:00.005", 0.005)
};

std::ostream & operator<< (std::ostream & os, valid_case_t const & val)
{
  return boost::tuples::operator<<(os, val);
}

class ValidClockValue : public ::testing::TestWithParam<valid_case_t> {
};

char const * const InvalidStrings[] = {
  //"1:60:00", TODO: add check
  "1:10:60",
  "9:34",
  "15mi"
};

}

TEST_P(ValidClockValue, Valid)
{
  std::string testStr = GetParam().get<0>();
  valid_testT(testStr, GetParam().get<1>());
  valid_testT(std::wstring(testStr.begin(), testStr.end()), GetParam().get<1>());
}

INSTANTIATE_TEST_CASE_P(clock_value_grammar,
                        ValidClockValue,
                        ::testing::ValuesIn(ValidTests));


class InvalidClockValue : public ::testing::TestWithParam<const char*> {
};

TEST_P(InvalidClockValue, Invalid)
{
  std::string testStr = GetParam();
  invalid_testT(testStr);
  invalid_testT(std::wstring(testStr.begin(), testStr.end()) );
}

INSTANTIATE_TEST_CASE_P(clock_value_grammar,
                        InvalidClockValue,
                        ::testing::ValuesIn(InvalidStrings));
