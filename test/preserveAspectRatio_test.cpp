#include <svgpp/parser/misc.hpp>

#include <gtest/gtest.h>
#include <sstream>
#include <boost/noncopyable.hpp>

namespace
{
  struct Context: boost::noncopyable
  {
    void set(svgpp::tag::attribute::preserveAspectRatio, bool defer,
      svgpp::tag::value::none)
    {
      os_ << (defer ? "defer " : "") << "none";
    }

#define ON_VALUE(xy_value) \
    template<class MeetOrSliceTag> \
    void set(svgpp::tag::attribute::preserveAspectRatio, bool defer, \
      svgpp::tag::value::xy_value, MeetOrSliceTag meetOrSlice) \
    { \
    os_ << (defer ? "defer " : "") << #xy_value; \
      outMeetOrSlice(meetOrSlice); \
    } 

    ON_VALUE(xMinYMin)
    ON_VALUE(xMidYMin)
    ON_VALUE(xMaxYMin)
    ON_VALUE(xMinYMid)
    ON_VALUE(xMidYMid)
    ON_VALUE(xMaxYMid)
    ON_VALUE(xMinYMax)
    ON_VALUE(xMidYMax)
    ON_VALUE(xMaxYMax)

    std::string output() const
    {
      return os_.str();
    }

  private:
    std::ostringstream os_;

    void outMeetOrSlice(svgpp::tag::value::slice)
    {
      os_ << " slice";
    }

    void outMeetOrSlice(svgpp::tag::value::meet)
    {
      os_ << " meet";
    }
  };

  void valid_test(std::string const & value, std::string const & expectedOutput)
  {
    Context context;
    EXPECT_TRUE(svgpp::value_parser<svgpp::tag::attribute::preserveAspectRatio>::parse(
      svgpp::tag::attribute::preserveAspectRatio(),
      context,
      value,
      svgpp::tag::source::attribute()));
    EXPECT_EQ(context.output(), expectedOutput);
  }

  void invalid_test(std::string const & value)
  {
    Context context;
    EXPECT_THROW(svgpp::value_parser<svgpp::tag::attribute::preserveAspectRatio>::parse(
      svgpp::tag::attribute::preserveAspectRatio(),
      context,
      value,
      svgpp::tag::source::attribute()),
      std::exception);
  }

  typedef std::pair<std::string, std::string> test_case_t;
          
  test_case_t ValidCases[] = {
    test_case_t("defer none meet", "defer none"),
    test_case_t("defer none slice", "defer none"),
    test_case_t("defer none", "defer none"),
    test_case_t("none", "none"),
    test_case_t("xMinYMin  meet", "xMinYMin meet"),
    test_case_t("xMinYMin  slice", "xMinYMin slice"),
    test_case_t("xMinYMin", "xMinYMin meet"),
    test_case_t("xMidYMin", "xMidYMin meet"),
    test_case_t("xMaxYMin", "xMaxYMin meet"),
    test_case_t("xMinYMid", "xMinYMid meet"),
    test_case_t("xMidYMid", "xMidYMid meet"),
    test_case_t("xMaxYMid", "xMaxYMid meet"),
    test_case_t("xMinYMax", "xMinYMax meet"),
    test_case_t("xMidYMax", "xMidYMax meet"),
    test_case_t("xMaxYMax", "xMaxYMax meet"),
  };             

  std::string InvalidCases[] = {
    "",
    "nonemeet",
    "defer nonemeet",
    "defer none meet ",
    " defer none",
    " xMinYMin",
    "xMinYMin "
  };
}

class preserveAspectRatio_valid : public ::testing::TestWithParam<test_case_t> {
};

TEST_P(preserveAspectRatio_valid, parse)
{
  valid_test(GetParam().first, GetParam().second);
}

INSTANTIATE_TEST_CASE_P(value_parser,
                        preserveAspectRatio_valid,
                        ::testing::ValuesIn(ValidCases));

class preserveAspectRatio_invalid : public ::testing::TestWithParam<std::string> {
};

TEST_P(preserveAspectRatio_invalid, parse)
{
  invalid_test(GetParam());
}

INSTANTIATE_TEST_CASE_P(value_parser,
                        preserveAspectRatio_invalid,
                        ::testing::ValuesIn(InvalidCases));