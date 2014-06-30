#include <svgpp/parser/grammar/path.hpp>
#include <svgpp/path_adapter.hpp>

#include "test_path_context.hpp"

#include <gtest/gtest.h>

struct TestContextDirect: std::ostringstream {};
struct TestContextAbs: std::ostringstream {};

namespace svgpp
{
  template<>
  struct path_context_traits<TestContextAbs>: path_context_traits_raw<>
  {
    static const bool absolute_coordinates_only = true;
  };
}

TEST(path_adapter, absolute_coordinates_only)
{
  namespace qi = boost::spirit::qi;
  using boost::phoenix::ref;
  using namespace svgpp;

  std::string pathStr(
    "M300,200 100 200 h-150za150,151 0 1,0 150,-150z"
    "M300,200 100 200 h-150a150,151 0 1,0 150,-150z"
    "M100,200 C100,100 250,100 250,200S400,300 400-200"
    "m20 40 50 30");
  std::ostringstream expected;
  typedef test_path_context_traits<std::ostringstream> traits;
  // Line 1
  traits::path_move_to(expected, 300, 200, absolute_coordinate_tag());
  traits::path_line_to(expected, 100, 200, absolute_coordinate_tag());
  traits::path_line_to_ortho(expected, -50, true, absolute_coordinate_tag());
  traits::path_close_path(expected);
  traits::path_elliptical_arc_to(expected, 150, 151, 0, true, false, 450, 50, absolute_coordinate_tag());
  traits::path_close_path(expected);
  // Line 2
  traits::path_move_to(expected, 300, 200, absolute_coordinate_tag());
  traits::path_line_to(expected, 100, 200, absolute_coordinate_tag());
  traits::path_line_to_ortho(expected, -50, true, absolute_coordinate_tag());
  traits::path_elliptical_arc_to(expected, 150, 151, 0, true, false, 100, 50, absolute_coordinate_tag());
  traits::path_close_path(expected);
  // Line3
  traits::path_move_to(expected, 100, 200, absolute_coordinate_tag());
  traits::path_cubic_bezier_to(expected, 100, 100, 250, 100, 250, 200, absolute_coordinate_tag());
  traits::path_cubic_bezier_to(expected, 400, 300, 400, -200, absolute_coordinate_tag());
  traits::path_move_to(expected, 420, -160, absolute_coordinate_tag());
  traits::path_line_to(expected, 470, -130, absolute_coordinate_tag());
  std::string::const_iterator first = pathStr.begin();
  TestContextAbs context1;
  {
    choose_path_adapter<TestContextAbs>::type adapted_context(context1);
    svgpp::spirit::path_grammar<std::string::const_iterator, 
      choose_path_adapter<TestContextAbs>::type,
      choose_path_adapter<TestContextAbs>::traits_type> grammar;
    BOOST_CHECK(qi::phrase_parse(first, pathStr.cend(), grammar(boost::phoenix::ref(adapted_context)), 
      boost::spirit::ascii::space));
  }
  EXPECT_TRUE(first == pathStr.end());
  EXPECT_EQ(expected.str(), context1.str());
}
