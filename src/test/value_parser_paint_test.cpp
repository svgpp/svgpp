#include <svgpp/parser/paint.hpp>

#include <gtest/gtest.h>
#include <sstream>

namespace
{
  using namespace svgpp;

  class Context
  {
  public:
    std::string str() const { return str_.str(); }

    void set(tag::attribute::stroke, tag::value::none)
    {
      str_ << "[none]";
    }

    void set(tag::attribute::stroke, tag::value::currentColor)
    {
      str_ << "[currentColor]";
    }

    void set(tag::attribute::stroke, int rgb, tag::skip_icc_color = tag::skip_icc_color())
    {
      str_ << "[rgb:" << std::hex << std::setw(6) << std::setfill('0') << rgb << "]";
    }

    typedef boost::iterator_range<std::string::const_iterator> iri_t;

    void set(tag::attribute::stroke, iri_t const & iri)
    {
      str_ << "[iri:" << iri << "]";
    }

    void set(tag::attribute::stroke, iri_t const & iri, tag::value::none)
    {
      str_ << "[iri:" << iri << "[none]]";
    }

    void set(tag::attribute::stroke, iri_t const & iri, tag::value::currentColor)
    {
      str_ << "[iri:" << iri << "[currentColor]]";
    }

    void set(tag::attribute::stroke, iri_t const & iri, int rgb, tag::skip_icc_color = tag::skip_icc_color())
    {
      str_ << "[iri:" << iri 
        << "[rgb:" << std::hex << std::setw(6) << std::setfill('0') << rgb << "]]";
    }

  private:
    std::ostringstream str_;
  };

  typedef std::pair<const char *, const char *> valid_case_t;

  valid_case_t ValidTests[] = {
    valid_case_t("none", "[none]"),
    valid_case_t("currentColor", "[currentColor]"),
    valid_case_t("inherit", ""),
    valid_case_t("rgb(11,12,240)", "[rgb:0b0cf0]"),
    valid_case_t("url(http://aa/b)", "[iri:http://aa/b]"),
    valid_case_t("url(http://aa/b) none", "[iri:http://aa/b[none]]"),
    valid_case_t("url(http://aa/b) currentColor", "[iri:http://aa/b[currentColor]]"),
    valid_case_t("url(#xxxx)  rgb(11,12,240)", "[iri:#xxxx[rgb:0b0cf0]]"),
  };

  valid_case_t ValidTestsCSS[] = {
    valid_case_t("NoNe", "[none]"),
    valid_case_t("CURRENTCOLOR", "[currentColor]"),
    valid_case_t("RGB(11,12,240)", "[rgb:0b0cf0]")
  };
}

class ValidPaint : public ::testing::TestWithParam<valid_case_t> {
};

TEST_P(ValidPaint, Valid)
{
  std::string testStr = GetParam().first;
  std::string expectedStr = GetParam().second;

  Context ctx;
  EXPECT_TRUE((value_parser<tag::type::paint, iri_policy<policy::iri::raw> >::parse(
    tag::attribute::stroke(), ctx, testStr, tag::source::attribute())));
  EXPECT_EQ(expectedStr, ctx.str());
}

INSTANTIATE_TEST_CASE_P(value_parser,
                        ValidPaint,
                        ::testing::ValuesIn(ValidTests));

class ValidPaintCSS : public ::testing::TestWithParam<valid_case_t> {
};

TEST_P(ValidPaintCSS, Valid)
{
  std::string testStr = GetParam().first;
  std::string expectedStr = GetParam().second;

  Context ctx;
  EXPECT_TRUE((value_parser<tag::type::paint, iri_policy<policy::iri::raw> >::parse(
    tag::attribute::stroke(), ctx, testStr, tag::source::css())));
  EXPECT_EQ(expectedStr, ctx.str());
}

INSTANTIATE_TEST_CASE_P(value_parser,
                        ValidPaintCSS,
                        ::testing::ValuesIn(ValidTestsCSS));