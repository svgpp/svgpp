#include <svgpp/parser/grammar/icc_color.hpp>

#include <gtest/gtest.h>
#include <boost/assign/list_of.hpp>

namespace qi = boost::spirit::qi;

namespace
{

template<class StringType>
struct icc_color_factory
{
  typedef double component_type;
  typedef std::pair<StringType, std::vector<component_type> > icc_color_type;
  typedef icc_color_type builder_type;

  template<class Iterator>
  static void set_profile_name(builder_type & b, typename boost::iterator_range<Iterator> const & r) 
  {
    EXPECT_TRUE(b.first.empty());
    b.first.assign(boost::begin(r), boost::end(r));
  }
  static void append_component_value(builder_type & b, component_type val) 
  {
    b.second.push_back(val);
  }

  static icc_color_type create_icc_color(builder_type const & b) { return b; }
};

template<class StringT>
void valid_testT(StringT const & testStr, StringT const & expectedProfileName, std::vector<double> const & components)
{
  typename StringT::const_iterator first = testStr.begin();
  typedef icc_color_factory<StringT> icc_color_factory_t;
  typedef svgpp::icc_color_grammar<
    svgpp::tag::source::attribute, 
    typename StringT::const_iterator, icc_color_factory_t
  > grammar_t;
  grammar_t grammar;
  icc_color_factory_t factory;
  typename icc_color_factory_t::icc_color_type color;
  EXPECT_TRUE(qi::parse(first, testStr.end(), grammar(boost::phoenix::ref(factory)), color));
  EXPECT_TRUE(first == testStr.end());
  EXPECT_EQ(expectedProfileName, color.first);
  EXPECT_EQ(components, color.second);
}

template<class StringT>
void invalid_testT(StringT const & testStr)
{
  typename StringT::const_iterator first = testStr.begin();
  svgpp::icc_color_grammar<svgpp::tag::source::attribute, typename StringT::const_iterator, icc_color_factory<StringT> > grammar;
  icc_color_factory<StringT> factory;
  EXPECT_TRUE(!qi::parse(first, testStr.end(), grammar(boost::phoenix::ref(factory))) || first != testStr.end());
}

typedef boost::tuple<const char *, const char *, std::vector<double> > valid_case_t;

using namespace boost::assign;

valid_case_t ValidColorTests[] = {
  valid_case_t("icc-color(profile-name 3.7)", "profile-name", list_of(3.7)),
  valid_case_t("icc-color(profile-name,3.7)", "profile-name", list_of(3.7)),
  valid_case_t("icc-color(profile[name] , 3.7e-5,-.5)", "profile[name]", list_of(3.7e-5)(-.5)),
  valid_case_t("icc-color(profile[name] , 3.7e-5 -.5)", "profile[name]", list_of(3.7e-5)(-.5)),
};

std::ostream & operator<< (std::ostream & os, valid_case_t const & val)
{
  os << "\"" << val.get<0>() << "\"";
  return os;
}

class ValidICCColor : public ::testing::TestWithParam<valid_case_t> {
};

char const * const InvalidColorStrings[] = {
  "icccolor(profile-name 3.7)",
  "icc-color(profile-name 3.7) ",
  "icc-color( profile-name 3.7)",
  "icc-color(profile-name 3.7 )",
  "icc-color(profile name 3.7)",
  "icc-color(profilename,, 3.7)",
  "icc-color(profilename, 3.7 4.8,)",
};

}

TEST_P(ValidICCColor, Valid)
{
  std::string testStr = GetParam().get<0>();
  std::string expectedStr = GetParam().get<1>();
  valid_testT(testStr, expectedStr, GetParam().get<2>());
  valid_testT(std::wstring(testStr.begin(), testStr.end()), 
    std::wstring(expectedStr.begin(), expectedStr.end()), 
    GetParam().get<2>());
}

INSTANTIATE_TEST_CASE_P(icc_color_grammar,
                        ValidICCColor,
                        ::testing::ValuesIn(ValidColorTests));


class InvalidICCColor : public ::testing::TestWithParam<const char*> {
};

TEST_P(InvalidICCColor, Invalid)
{
  std::string testStr = GetParam();
  invalid_testT(testStr);
  invalid_testT(std::wstring(testStr.begin(), testStr.end()) );
}

INSTANTIATE_TEST_CASE_P(icc_color_grammar,
                        InvalidICCColor,
                        ::testing::ValuesIn(InvalidColorStrings));
