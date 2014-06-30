#include <svgpp/value_parsers/spirit/list_of_points_grammar.hpp>

#include <boost/test/unit_test.hpp>

namespace qi = boost::spirit::qi;

struct TestContext: std::ostringstream {};

inline void append_list_point(TestContext & context, double x, double y)
{
  context << "(" << x << "," << y << ")";
}

BOOST_AUTO_TEST_CASE(list_of_points_grammar)
{
  using boost::phoenix::ref;
  using namespace svgpp;
  std::string testStr("12.3 14.7,34.,176 77-67.3   \n2 ,  \n1 ,-3-4");
  TestContext expected;
  append_list_point(expected, 12.3, 14.7);
  append_list_point(expected, 34, 176);
  append_list_point(expected, 77, -67.3);
  append_list_point(expected, 2, 1);
  append_list_point(expected, -3, -4);

  std::string::const_iterator first = testStr.begin();
  TestContext context1;
  svgpp::spirit::list_of_points_grammar<std::string::const_iterator, TestContext> grammar;
  BOOST_CHECK(qi::phrase_parse(first, testStr.cend(), grammar(boost::phoenix::ref(context1)), boost::spirit::ascii::space));
  BOOST_CHECK(first == testStr.end());
  BOOST_CHECK_EQUAL(expected.str(), context1.str());
  {
    std::string testStr2("12 14-77 67");
    std::string::const_iterator first2 = testStr2.begin();
    BOOST_CHECK(!qi::phrase_parse(first2, testStr2.cend(), grammar(boost::phoenix::ref(context1)), boost::spirit::ascii::space)
      || first2 != testStr2.end());
  }
  {
    std::string testStr2("12.3 14.7,");
    std::string::const_iterator first2 = testStr2.begin();
    BOOST_CHECK(!qi::phrase_parse(first2, testStr2.cend(), grammar(boost::phoenix::ref(context1)), boost::spirit::ascii::space)
      || first2 != testStr2.end());
  }
  {
    std::string testStr2(",12.3 14.7");
    std::string::const_iterator first2 = testStr2.begin();
    BOOST_CHECK(!qi::phrase_parse(first2, testStr2.cend(), grammar(boost::phoenix::ref(context1)), boost::spirit::ascii::space)
      || first2 != testStr2.end());
  }
}