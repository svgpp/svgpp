#include <svgpp/svgpp.hpp>
#include <boost/version.hpp>
#if BOOST_VERSION >= 106400
#include <boost/serialization/array_wrapper.hpp>
#endif
#include <boost/math/constants/constants.hpp>
#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace svgpp;
namespace ublas = boost::numeric::ublas;

typedef ublas::matrix<double> matrix_t;

struct TransformEventsPolicy
{
  typedef matrix_t context_type;

  static void transform_matrix(matrix_t & transform, const boost::array<double, 6> & matrix)
  {
    matrix_t m(3, 3);
    m <<=
      matrix[0], matrix[2], matrix[4],
      matrix[1], matrix[3], matrix[5],
      0, 0, 1;
    transform = ublas::prod(transform, m);
  }

  static void transform_translate(matrix_t & transform, double tx, double ty)
  {
    matrix_t m = ublas::identity_matrix<double>(3, 3);
    m(0, 2) = tx; m(1, 2) = ty;
    transform = ublas::prod(transform, m);
  }

  static void transform_scale(matrix_t & transform, double sx, double sy)
  {
    matrix_t m = ublas::identity_matrix<double>(3, 3);
    m(0, 0) = sx; m(1, 1) = sy; 
    transform = ublas::prod(transform, m);
  }

  static void transform_rotate(matrix_t & transform, double angle)
  {
    angle *= boost::math::constants::degree<double>();
    matrix_t m(3, 3);
    m <<=
      std::cos(angle), -std::sin(angle), 0,
      std::sin(angle),  std::cos(angle), 0,
      0, 0, 1;
    transform = ublas::prod(transform, m);
  }

  static void transform_skew_x(matrix_t & transform, double angle)
  {
    angle *= boost::math::constants::degree<double>();
    matrix_t m = ublas::identity_matrix<double>(3, 3);
    m(0, 1) = std::tan(angle);
    transform = ublas::prod(transform, m);
  }

  static void transform_skew_y(matrix_t & transform, double angle)
  {
    angle *= boost::math::constants::degree<double>();
    matrix_t m = ublas::identity_matrix<double>(3, 3);
    m(1, 0) = std::tan(angle);
    transform = ublas::prod(transform, m);
  }
};

int main()
{
  matrix_t transform(ublas::identity_matrix<double>(3, 3));
  value_parser<
    tag::type::transform_list,
    transform_policy<policy::transform::minimal>,
    transform_events_policy<TransformEventsPolicy>
  >::parse(tag::attribute::transform(), transform,
    std::string("translate(-10,-20) scale(2) rotate(45) translate(5,10)"), tag::source::attribute());
  std::cout << transform << "\n";
  return 0;
}
