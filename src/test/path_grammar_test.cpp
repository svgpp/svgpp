#include <svgpp/parser/grammar/path_data.hpp>
#include <sstream>

#include "test_path_context.hpp"

#include <gtest/gtest.h>

struct TestContext: std::ostringstream {};

TEST(path_data_grammar, p1)
{
  namespace qi = boost::spirit::qi;
  using boost::phoenix::ref;
  using namespace svgpp;
  
  std::string pathStr1("M300,200 100 200 h-150za150,151 0 1,0 150,-150z"
    "M100,200 C100,100 250,100 250,200S400,300 400-200");
  test_path_context expected_context;
  expected_context.path_move_to(300, 200, tag::coordinate::absolute());
  expected_context.path_line_to(100, 200, tag::coordinate::absolute());
  expected_context.path_line_to_ortho(-150, true, tag::coordinate::relative());
  expected_context.path_close_subpath();
  expected_context.path_elliptical_arc_to(150, 151, 0, true, false, 150, -150, tag::coordinate::relative());
  expected_context.path_close_subpath();
  expected_context.path_move_to(100, 200, tag::coordinate::absolute());
  expected_context.path_cubic_bezier_to(100, 100, 250, 100, 250, 200, tag::coordinate::absolute());
  expected_context.path_cubic_bezier_to(400, 300, 400, -200, tag::coordinate::absolute());

  std::string::const_iterator first = pathStr1.begin(), end = pathStr1.end();
  test_path_context test_context;
  svgpp::path_data_grammar<std::string::const_iterator, test_path_context> grammar;
  EXPECT_TRUE(qi::phrase_parse(first, end, grammar(boost::phoenix::ref(test_context)), boost::spirit::ascii::space));
  EXPECT_TRUE(first == pathStr1.end());
  EXPECT_EQ(expected_context.str(), test_context.str());
}

TEST(path_data_grammar, p2)
{
  namespace qi = boost::spirit::qi;
  using boost::phoenix::ref;
  using namespace svgpp;
  
  std::string pathStr1("M30.262 57.02L7.195 40.723c-5.84-3.976-7.56-12.06-3.842-18.063 3.715-6 11.467-7.65 17.306-3.68l4.52 3.76 2.6-5.274c3.717-6.002 11.47-7.65 17.305-3.68 5.84 3.97 7.56 12.054 3.842 18.062L34.49 56.118c-.897 1.512-2.793 1.915-4.228.9z");

  std::string::const_iterator first = pathStr1.begin(), end = pathStr1.end();
  test_path_context test_context;
  svgpp::path_data_grammar<std::string::const_iterator, test_path_context> grammar;
  EXPECT_TRUE(qi::phrase_parse(first, end, grammar(boost::phoenix::ref(test_context)), boost::spirit::ascii::space));
  EXPECT_TRUE(first == pathStr1.end());
}
