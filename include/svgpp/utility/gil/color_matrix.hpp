// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <cmath>
#include <boost/gil/color_base_algorithm.hpp>
#include <boost/gil/rgba.hpp>
#include <boost/multi_array.hpp>

namespace svgpp { namespace gil_utility {

template<class Color, class Scalar = double>
class color_matrix_transform
{
public:
  typedef boost::multi_array<Scalar, 2> matrix_t;

  color_matrix_transform(matrix_t const & matrix)
    : matrix_(matrix)
  {
    if (matrix.shape()[0] == 3 && matrix.shape()[1] == 3)
    {
      // TODO make specialization for 3x3 matrix
      matrix_.resize(boost::extents[4][5]);
      matrix_[3][3] = 1;
    }
    else
      BOOST_ASSERT(matrix.shape()[0] == 4 && matrix.shape()[1] == 5);
  }

  Color operator()(const Color & color) const 
  {
    // TODO integer optimization
    namespace gil = boost::gil;
    Color result;
    typename gil::color_element_type<Color, gil::red_t>::type 
      r = gil::get_color(color, gil::red_t()),
      g = gil::get_color(color, gil::green_t()),
      b = gil::get_color(color, gil::blue_t()),
      a = gil::get_color(color, gil::alpha_t());
    gil::get_color(result, gil::red_t())    = matrix_[0][0] * r + matrix_[0][1] * g + matrix_[0][2] * b + matrix_[0][3] * a + matrix_[0][4];
    gil::get_color(result, gil::green_t())  = matrix_[1][0] * r + matrix_[1][1] * g + matrix_[1][2] * b + matrix_[1][3] * a + matrix_[1][4];
    gil::get_color(result, gil::blue_t())   = matrix_[2][0] * r + matrix_[2][1] * g + matrix_[2][2] * b + matrix_[2][3] * a + matrix_[2][4];
    gil::get_color(result, gil::alpha_t())  = matrix_[3][0] * r + matrix_[3][1] * g + matrix_[3][2] * b + matrix_[3][3] * a + matrix_[3][4];
    return result;
  }

private:
  matrix_t matrix_;
};

// A saturate operation is equivalent to the following matrix operation:
// | R' |     |0.213+0.787s  0.715-0.715s  0.072-0.072s 0  0 |   | R |
// | G' |     |0.213-0.213s  0.715+0.285s  0.072-0.072s 0  0 |   | G |
// | B' |  =  |0.213-0.213s  0.715-0.715s  0.072+0.928s 0  0 | * | B |
// | A' |     |           0            0             0  1  0 |   | A |
// | 1  |     |           0            0             0  0  1 |   | 1 |

template<class Scalar>
boost::multi_array<Scalar, 2> get_saturate_matrix(Scalar s)
{
  boost::multi_array<Scalar, 2> m(boost::extents[3][3]);
  m[0][0] = 0.213+0.787*s; m[0][1] = 0.715-0.715*s; m[0][2] = 0.072-0.072*s;
  m[1][0] = 0.213-0.213*s; m[1][1] = 0.715+0.285*s; m[1][2] = 0.072-0.072*s;
  m[2][0] = 0.213-0.213*s; m[2][1] = 0.715-0.715*s; m[2][2] = 0.072+0.928*s;
  return m;
}

// A hueRotate operation is equivalent to the following matrix operation:
// | a00 a01 a02 |    [+0.213 +0.715 +0.072]                            [+0.787 -0.715 -0.072]                            [-0.213 -0.715+0.928]
// | a10 a11 a12 | =  [+0.213 +0.715 +0.072] +  cos(hueRotate value) *  [-0.213 +0.285 -0.072] +  sin(hueRotate value) *  [+0.143 +0.140-0.283]
// | a20 a21 a22 |    [+0.213 +0.715 +0.072]                            [-0.213 -0.715 +0.928]                            [-0.787 +0.715+0.072]

template<class Scalar>
boost::multi_array<Scalar, 2> get_hue_rotate_matrix(Scalar hueRotateRad)
{
  Scalar hue_sin = std::sin(hueRotateRad);
  Scalar hue_cos = std::cos(hueRotateRad);
  Scalar const m1[3][3] = {
    +0.213, +0.715, +0.072,
    +0.213, +0.715, +0.072,
    +0.213, +0.715, +0.072 };
  Scalar const m2[3][3] = {
    +0.787, -0.715, -0.072,
    -0.213, +0.285, -0.072,
    -0.213, -0.715, +0.928 };
  Scalar const m3[3][3] = {
    -0.213, -0.715, +0.928,
    +0.143, +0.140, -0.283,
    -0.787, +0.715, +0.072 };
  boost::multi_array<Scalar, 2> m(boost::extents[3][3]);
  for(int i = 0; i < 3; ++i)
    for(int j = 0; j < 3; ++j)
      m[i][j] = m1[i][j] + hue_cos * m2[i][j] + hue_sin * m3[i][j];
  return m;
}

}}