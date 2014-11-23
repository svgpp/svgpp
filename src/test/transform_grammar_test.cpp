#include <svgpp/parser/grammar/transform.hpp>

#include <gtest/gtest.h>

struct test_transform_context
{
  inline void transform_matrix(boost::array<double, 6> const & m)
  {
    log_ << "matrix(";
    for(int i=0; i<6; ++i)
      log_<< m[i] << (i == 5 ? ")" : ",");
  }

  inline void transform_translate(double tx, double ty)
  {
    log_ << "translate(" << tx << "," << ty << ")";
  }

  inline void transform_translate(double tx)
  {
    log_ << "translate(" << tx << ")";
  }

  inline void transform_scale(double sx, double sy)
  {
    log_ << "scale(" << sx << "," << sy << ")";
  }

  inline void transform_scale(double sx)
  {
    log_ << "scale(" << sx << ")";
  }

  inline void transform_rotate(double a, double cx, double cy)
  {
    log_ << "rotate(" << a << "," << cx << "," << cy << ")";
  }

  inline void transform_rotate(double a)
  {
    log_ << "scale(" << a << ")";
  }

  inline void transform_skew_x(double a)
  {
    log_ << "skewX(" << a << ")";
  }

  inline void transform_skew_y(double a)
  {
    log_ << "skewY(" << a << ")";
  }
  std::string str() const { return log_.str(); }

private:
  std::ostringstream log_;
};

TEST(transform_grammar, t1)
{
  using boost::phoenix::ref;
  using namespace svgpp;
  std::string const testStr("translate(-10,-20) scale(2), ,rotate(45),translate(5,10)  matrix(1 2 3 4,5 ,6) translate(5.4),,rotate( 67.2,345,768) scale (-98 99e2 ) skewX(3.2) skewY(999)");
  test_transform_context expected;
  expected.transform_translate(-10, -20);
  expected.transform_scale(2);
  expected.transform_rotate(45);
  expected.transform_translate(5, 10);
  boost::array<double, 6> matrix = {{1, 2, 3, 4, 5, 6}};
  expected.transform_matrix(matrix);
  expected.transform_translate(5.4);
  expected.transform_rotate(67.2, 345, 768);
  expected.transform_scale(-98, 99e2);
  expected.transform_skew_x(3.2);
  expected.transform_skew_y(999);

  std::string::const_iterator first = testStr.begin();
  test_transform_context test_context;
  svgpp::transform_grammar<std::string::const_iterator, test_transform_context> grammar;
  EXPECT_TRUE(qi::phrase_parse(first, testStr.end(), grammar(boost::phoenix::ref(test_context)), boost::spirit::ascii::space));
  EXPECT_TRUE(first == testStr.end());
  EXPECT_EQ(expected.str(), test_context.str());
  {
    std::string const testStr2("translate(-10,-20)scale(2)");
    std::string::const_iterator first2 = testStr2.begin();
    EXPECT_TRUE(!qi::phrase_parse(first2, testStr2.end(), grammar(boost::phoenix::ref(test_context)), boost::spirit::ascii::space)
      || first2 != testStr2.end());
  }
  {
    std::string const testStr2("12.3 14.7,");
    std::string::const_iterator first2 = testStr2.begin();
    EXPECT_TRUE(!qi::phrase_parse(first2, testStr2.end(), grammar(boost::phoenix::ref(test_context)), boost::spirit::ascii::space)
      || first2 != testStr2.end());
  }
  {
    std::string const testStr2(",12.3 14.7");
    std::string::const_iterator first2 = testStr2.begin();
    EXPECT_TRUE(!qi::phrase_parse(first2, testStr2.end(), grammar(boost::phoenix::ref(test_context)), boost::spirit::ascii::space)
      || first2 != testStr2.end());
  }
}