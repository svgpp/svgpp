#include <svgpp/parser/length.hpp>
#include <svgpp/traits/length_units.hpp>

#include <gtest/gtest.h>
#include <boost/assign/list_of.hpp>
#include <boost/range/any_range.hpp>

namespace svgpp { namespace tag { namespace length_units {

#define ON_UNIT(unit) \
  bool operator== (unit const &, unit const &) { return true; } \
  std::ostream & operator<<(std::ostream & os, unit) \
  { os << "<" << #unit << ">"; return os; }
  
ON_UNIT(in)
ON_UNIT(cm)
ON_UNIT(mm)
ON_UNIT(pt)
ON_UNIT(pc)
ON_UNIT(em)
ON_UNIT(ex)
ON_UNIT(px)
ON_UNIT(percent)
ON_UNIT(none)

}}}

namespace
{

using namespace boost::assign;
using namespace svgpp::tag::length_units;

/*template<class UnitsTag>
struct units_wrapper
{
  units_wrapper() {}
  units_wrapper(units_wrapper const &) {}
  units_wrapper & operator= (units_wrapper const &) { return *this; }
  bool operator== (units_wrapper const &) const { return true; }
};*/

typedef boost::variant<
  in,
  cm,
  mm,
  pt,
  pc,
  em,
  ex,
  px,
  percent,
  none
> length_units;

typedef std::pair<double, length_units> length_t;

struct length_factory_t
{
  typedef double number_type;
  typedef length_t length_type;

  length_factory_t() {}

  template<class UnitsTag>
  length_type create_length(double val, UnitsTag tag) const
  {
    return length_t(val, tag);
  }

  length_type create_length(double val, percent tag, svgpp::tag::length_dimension::any const &) const
  {
    return length_t(val, tag);
  }
};

struct length_policy
{
  typedef length_factory_t const length_factory_type;

  template<class Context>
  static length_factory_type & length_factory(Context const & context)
  {
    static const length_factory_t instance;
    return instance;
  }
};

typedef std::vector<length_t> lengths_t;
typedef std::pair<const char *, lengths_t> valid_case_t;

valid_case_t valid_tests[] = {
  valid_case_t("1 2", list_of(length_t(1, none()))(length_t(2, none())).convert_to_container<lengths_t>()),
  valid_case_t("4em 5.0ex", list_of(length_t(4, em()))(length_t(5, ex())).convert_to_container<lengths_t>() ),
  valid_case_t("1e-9px, 2e+10pt", list_of(length_t(1e-9, px()))(length_t(2e10, pt())).convert_to_container<lengths_t>() ),
  valid_case_t(".59px , 21.0cm", list_of(length_t(.59, px()))(length_t(21, cm())).convert_to_container<lengths_t>() ),
  valid_case_t("-56px -77pt", list_of(length_t(-56, px()))(length_t(-77, pt())).convert_to_container<lengths_t>() ),
};

const char * invalid_tests[] = {
  "1 2 ",
  " 1 2",
  "1,,2",
  "1 ,,2",
  "1, ,2",
  "1, 2,",
  ",1, 2",
  "1.1.2",
  "1-2",
  "1 px 2",
  "1,px 2",
  "1 px,2",
  "1px.4",
};

class list_of_lengths_valid : public ::testing::TestWithParam<valid_case_t> 
{};

class list_of_lengths_invalid : public ::testing::TestWithParam<const char *> 
{};

struct test_context
{
#if 0
  typedef boost::any_range<length_t, boost::single_pass_traversal_tag, length_t const &, std::ptrdiff_t> Range;
  void set(svgpp::tag::attribute::by, Range const & range)
#else
  template<class Range>
  void set(svgpp::tag::attribute::by, Range const & range)
#endif
  {
    for(typename boost::range_iterator<Range>::type it = boost::begin(range), end = boost::end(range); 
      it != end; ++it)
      values_.push_back(*it);
  }

  std::vector<length_t> values_;
};

}

TEST_P(list_of_lengths_valid, t1)
{
  test_context context;
  svgpp::value_parser<
    svgpp::tag::type::list_of<svgpp::tag::type::length>,
    svgpp::length_policy<length_policy>
  >::parse(svgpp::tag::attribute::by(), 
    context, std::string(GetParam().first), svgpp::tag::source::attribute());
  EXPECT_TRUE(context.values_ == GetParam().second);
}

INSTANTIATE_TEST_CASE_P(value_parser,
                        list_of_lengths_valid,
                        ::testing::ValuesIn(valid_tests));

TEST_P(list_of_lengths_invalid, t1)
{
}

INSTANTIATE_TEST_CASE_P(value_parser,
                        list_of_lengths_invalid,
                        ::testing::ValuesIn(invalid_tests));
