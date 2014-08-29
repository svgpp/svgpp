#include <svgpp/parser/list_of_points.hpp>

#include <gtest/gtest.h>
#include <boost/assign/list_of.hpp>
#include <boost/tuple/tuple_io.hpp>

namespace
{

typedef std::vector<std::pair<double, double> > pair_list_t;

struct Context
{
  template<class Range>
  void set(svgpp::tag::attribute::points, Range const & r)
  {
    for(typename boost::range_const_iterator<Range>::type it = boost::begin(r); it != boost::end(r); ++it)
      values_.push_back(*it);
  }

  pair_list_t values_;
};

typedef std::pair<const char *, pair_list_t> valid_case_t;

using namespace boost::assign;

valid_case_t valid_tests[] = {
  valid_case_t("  ", pair_list_t() ),
  valid_case_t("1 2", list_of(std::make_pair(1.0, 2.0)).convert_to_container<pair_list_t>() ),
  valid_case_t("12.3 14.7,34.,176 77-67.3   \n2 ,  \n1 ,-3-4", 
    list_of(std::make_pair(12.3, 14.7))
    (std::make_pair(34, 176))
    (std::make_pair(77, -67.3))
    (std::make_pair(2, 1))
    (std::make_pair(-3, -4)).convert_to_container<pair_list_t>()
    )
};

class list_of_points_valid : public ::testing::TestWithParam<valid_case_t> {
};

char const * const invalid_tests[] = {
  ",",
  "12 14-77 67",
  "12.3 14.7,",
  ",12.3 14.7",
  "12.3,,14.7"
};

class list_of_points_invalid : public ::testing::TestWithParam<const char *> 
{};

}

TEST_P(list_of_points_valid, valid)
{
  Context context;
  EXPECT_TRUE(svgpp::value_parser<svgpp::tag::attribute::points>::parse(
    svgpp::tag::attribute::points(),
    context,
    std::string(GetParam().first),
    svgpp::tag::source::attribute()
  ));
  EXPECT_EQ(context.values_, GetParam().second);
}

INSTANTIATE_TEST_CASE_P(value_parser,
                        list_of_points_valid,
                        ::testing::ValuesIn(valid_tests));


class InvalidURange : public ::testing::TestWithParam<const char*> {
};

TEST_P(list_of_points_invalid, invalid)
{
  Context context;
  EXPECT_THROW(svgpp::value_parser<svgpp::tag::attribute::points>::parse(
    svgpp::tag::attribute::points(),
    context,
    std::string(GetParam()),
    svgpp::tag::source::attribute()
  ), std::exception);
}

INSTANTIATE_TEST_CASE_P(value_parser,
                        list_of_points_invalid,
                        ::testing::ValuesIn(invalid_tests));
