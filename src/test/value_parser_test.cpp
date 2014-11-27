#include <svgpp/parser/number.hpp>

#include <limits>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/tuple/tuple.hpp>
#include <gtest/gtest.h>

using namespace svgpp;

namespace
{
  struct Context
  {
    Context()
      : value_(std::numeric_limits<double>::max())
    {}

    void set(tag::attribute::by, double val)
    {
      value_ = val;
    }

    double value_;
  };
}

TEST(value_parser, number)
{
  {
    Context ctx;
    EXPECT_TRUE(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("1234"), tag::source::attribute()));
    EXPECT_EQ(1234, ctx.value_);
    EXPECT_TRUE(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("+1234"), tag::source::attribute()));
    EXPECT_EQ(1234, ctx.value_);
    EXPECT_TRUE(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("-567"), tag::source::attribute()));
    EXPECT_EQ(-567, ctx.value_);
    EXPECT_TRUE(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("+0"), tag::source::attribute()));
    EXPECT_EQ(0, ctx.value_);
    EXPECT_TRUE(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("-.4"), tag::source::attribute()));
    EXPECT_EQ(-0.4, ctx.value_);
    EXPECT_TRUE(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("14.56"), tag::source::attribute()));
    EXPECT_EQ(14.56, ctx.value_);
    EXPECT_TRUE(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("-5.4e+10"), tag::source::attribute()));
    EXPECT_EQ(-5.4e+10, ctx.value_);
    EXPECT_TRUE(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("-5.4e-10"), tag::source::attribute()));
    EXPECT_EQ(-5.4e-10, ctx.value_);
    EXPECT_TRUE(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("7E10"), tag::source::attribute()));
    EXPECT_EQ(7e+10, ctx.value_);

    EXPECT_THROW(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("5."), tag::source::attribute()), std::exception);
    EXPECT_THROW(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("."), tag::source::attribute()), std::exception);
    EXPECT_THROW(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string(""), tag::source::attribute()), std::exception);
    EXPECT_THROW(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string(" "), tag::source::attribute()), std::exception);
    EXPECT_THROW(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("1N8"), tag::source::attribute()), std::exception);
    EXPECT_THROW(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("+-9"), tag::source::attribute()), std::exception);
    EXPECT_THROW(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string(" 5"), tag::source::attribute()), std::exception);
    EXPECT_THROW(value_parser<tag::type::number>::parse(tag::attribute::by(), ctx, std::string("5 "), tag::source::attribute()), std::exception);
  }
}

namespace
{
  typedef boost::tuple<const char *, double, boost::optional<double> > number_optional_number_test_t;

  class number_optional_number_valid : public ::testing::TestWithParam<number_optional_number_test_t> 
  {};

  number_optional_number_test_t number_optional_number_valid_tests[] = {
    number_optional_number_test_t("1e34 -3.5", 1e34, -3.5),
    number_optional_number_test_t("8e-4 ,-7.5", 8e-4, -7.5),
    number_optional_number_test_t("8e4 ,-7.5", 8e+4, -7.5),
    number_optional_number_test_t("8e4,-7.5", 8e+4, -7.5),
    number_optional_number_test_t("8e4, -7.5", 8e+4, -7.5),
    number_optional_number_test_t("8e4     -7.5", 8e+4, -7.5),
    number_optional_number_test_t("-6.12", -6.12)
  };

  std::ostream & operator<< (std::ostream & os, number_optional_number_test_t const & val)
  {
    os << "\"" << val.get<0>() << "\"";
    return os;
  }

  class number_optional_number_invalid : public ::testing::TestWithParam<const char *> 
  {};

  const char * number_optional_number_invalid_tests[] = {
    "-6.12 ",
    " -6.12",
    "-6.12 4.5 ",
    "-6.12 4.5 1",
    "-6.12,,4.5",
    "-6.12, ,4.5"
  };
}

namespace
{
  struct Context2
  {
    Context2()
      : value1_(std::numeric_limits<double>::max())
    {}

    void set(tag::attribute::by, double val)
    {
      value1_ = val;
    }

    void set(tag::attribute::by, double val, double val2)
    {
      value1_ = val;
      value2_ = val2;
    }

    double value1_;
    boost::optional<double> value2_;
  };
}

TEST_P(number_optional_number_valid, t1)
{
  {
    Context2 ctx;
    EXPECT_TRUE(value_parser<tag::type::number_optional_number>::parse(tag::attribute::by(), ctx, std::string(GetParam().get<0>()), tag::source::attribute()));
    EXPECT_EQ(GetParam().get<1>(), ctx.value1_);
    EXPECT_EQ(GetParam().get<2>(), ctx.value2_);
  }
}

INSTANTIATE_TEST_CASE_P(value_parser,
                        number_optional_number_valid,
                        ::testing::ValuesIn(number_optional_number_valid_tests));


namespace
{
  struct Context3
  {
    void set(tag::attribute::by, double) {}
    void set(tag::attribute::by, double, double) {}
  };
}

TEST_P(number_optional_number_invalid, t1)
{
  Context3 ctx;
  EXPECT_THROW(value_parser<tag::type::number_optional_number>::parse(tag::attribute::by(), ctx, std::string(GetParam()), tag::source::attribute()), std::exception);
}

INSTANTIATE_TEST_CASE_P(value_parser,
                        number_optional_number_invalid,
                        ::testing::ValuesIn(number_optional_number_invalid_tests));
