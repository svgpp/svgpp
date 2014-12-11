// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

// Based on article by Luc Maisonobe
// http://www.spaceroots.org/documents/ellipse/elliptical-arc.pdf

#pragma once

#include <boost/math/constants/constants.hpp>

namespace svgpp
{

namespace detail
{
  namespace
  {
    // error coefficients for cubic Bezier 
    template<class Number>
    struct arc_to_cubic_bezier_error_coefficients
    {
      static const Number mu[2][2][4][4];  // First index = 0 if (0 < b/a < 1/4), 1 if (1/4 <= b/a <= 1)
      static const Number safety[4];
    };

    template<class Number>
    const Number arc_to_cubic_bezier_error_coefficients<Number>::mu[2][2][4][4] =
    {
      {
        {
          {3.85268, -21.229, -0.330434, 0.0127842},
          {-1.61486, 0.706564, 0.225945, 0.263682},
          {-0.910164, 0.388383, 0.00551445, 0.00671814},
          {-0.630184, 0.192402, 0.0098871, 0.0102527}
        },
        {
          {-0.162211, 9.94329, 0.13723, 0.0124084},
          {-0.253135, 0.00187735, 0.0230286, 0.01264},
          {-0.0695069, -0.0437594, 0.0120636, 0.0163087},
          {-0.0328856, -0.00926032, -0.00173573, 0.00527385}
        },
      },
      {
        {
          {0.0899116, -19.2349, -4.11711, 0.183362},
          {0.138148, -1.45804, 1.32044, 1.38474},
          {0.230903, -0.450262, 0.219963, 0.414038},
          {0.0590565, -0.101062, 0.0430592, 0.0204699}
        },
        {
          {0.0164649, 9.89394, 0.0919496, 0.00760802},
          {0.0191603, -0.0322058, 0.0134667, -0.0825018},
          {0.0156192, -0.017535, 0.00326508, -0.228157},
          {-0.0236752, 0.0405821, -0.0173086, 0.176187}
        }
      }
    };

    template<class Number>
    const Number arc_to_cubic_bezier_error_coefficients<Number>::safety[4] =
      {0.001, 4.98, 0.207, 0.0067};
  }
}

template<class Number = double>
class arc_to_bezier
{
public:
  struct threshold_tag {};
  struct max_angle_tag {};
  struct circle_angle_tag {}; // If one thinks of an ellipse as a circle that has been stretched and then 
                              // rotated, then angles passed with this tag are angles on circle prior 
                              // to the stretch and rotate operations.

  // Full ellipse
  arc_to_bezier(Number cx, Number cy, Number a, Number b, Number theta, threshold_tag, Number threshold)
    : cx_(cx), cy_(cy), a_(a), b_(b)
  {
    eta1_ = 0;
    calculate_step(threshold_tag(), boost::math::constants::two_pi<Number>(), threshold);
    prepare(theta);
  }
  
  arc_to_bezier(Number cx, Number cy, Number a, Number b, Number theta, 
    Number lambda1, Number lambda2, threshold_tag, Number threshold)
    : cx_(cx), cy_(cy), a_(a), b_(b)
  {
    eta1_ = std::atan2(std::sin(lambda1)/b, std::cos(lambda1)/a);
    Number eta2 = std::atan2(std::sin(lambda2)/b, std::cos(lambda2)/a);
    calculate_step(threshold_tag(), eta2, threshold);
    prepare(theta);
  }

  arc_to_bezier(Number cx, Number cy, Number a, Number b, Number theta, 
    Number lambda1, Number lambda2, max_angle_tag, Number max_angle)
    : cx_(cx), cy_(cy), a_(a), b_(b)
  {
    // max_angle is not real angle, but is "eta" theoretical angle
    eta1_ = std::atan2(std::sin(lambda1)/b, std::cos(lambda1)/a);
    Number eta2 = std::atan2(std::sin(lambda2)/b, std::cos(lambda2)/a);
    calculate_step(max_angle_tag(), eta2, max_angle);
    prepare(theta);
  }

  arc_to_bezier(Number cx, Number cy, Number a, Number b, Number theta, 
    circle_angle_tag, Number eta1, Number eta2, max_angle_tag, Number max_angle)
    : cx_(cx), cy_(cy), a_(a), b_(b)
    , eta1_(eta1)
  {
    // max_angle is not real angle, but is "eta" theoretical angle
    calculate_step(max_angle_tag(), eta2, max_angle);
    prepare(theta);
  }

  int size() const { return size_; }

  class iterator
  {
  public:
    iterator(arc_to_bezier const & owner)
      : owner_(owner)
      , eta2_(owner.eta1_)
    {
      get_e_and_deriv(eta2_, p3x_, p3y_, alpha_e2_deriv_x_, alpha_e2_deriv_y_);
      alpha_e2_deriv_x_ *= owner_.alpha_;
      alpha_e2_deriv_y_ *= owner_.alpha_;
      i_ = -1;
      advance();
    }

    bool eof() const
    {
      return i_ == owner_.size_;
    }

    void advance()
    {
      ++i_;
      if (eof())
        return;
      p0x_ = p3x_;
      p0y_ = p3y_;
      eta2_ += owner_.deta_;
      p1x_ = p0x_ + alpha_e2_deriv_x_;
      p1y_ = p0y_ + alpha_e2_deriv_y_;

      get_e_and_deriv(eta2_, p3x_, p3y_, alpha_e2_deriv_x_, alpha_e2_deriv_y_);
      alpha_e2_deriv_x_ *= owner_.alpha_;
      alpha_e2_deriv_y_ *= owner_.alpha_;
      p2x_ = p3x_ - alpha_e2_deriv_x_;
      p2y_ = p3y_ - alpha_e2_deriv_y_;
    }

    Number p0x() const { return p0x_; }
    Number p0y() const { return p0y_; }
    Number p1x() const { return p1x_; }
    Number p1y() const { return p1y_; }
    Number p2x() const { return p2x_; }
    Number p2y() const { return p2y_; }
    Number p3x() const { return p3x_; }
    Number p3y() const { return p3y_; }

  private:
    arc_to_bezier const & owner_;
    int i_;
    Number eta2_;
    Number alpha_e2_deriv_x_, alpha_e2_deriv_y_;
    Number p0x_, p0y_;
    Number p1x_, p1y_;
    Number p2x_, p2y_;
    Number p3x_, p3y_;

    void get_e_and_deriv(Number eta, Number & ex, Number & ey, Number & e_deriv_x, Number & e_deriv_y) const
    {
      Number sin_eta = std::sin(eta);
      Number cos_eta = std::cos(eta);
      Number a_cos_eta = owner_.a_ * cos_eta;
      Number b_sin_eta = owner_.b_ * sin_eta;
      ex = owner_.cx_ + a_cos_eta * owner_.cos_theta_ - b_sin_eta * owner_.sin_theta_;
      ey = owner_.cy_ + a_cos_eta * owner_.sin_theta_ + b_sin_eta * owner_.cos_theta_;

      Number a_sin_eta = owner_.a_ * sin_eta;
      Number b_cos_eta = owner_.b_ * cos_eta;
      e_deriv_x = -a_sin_eta * owner_.cos_theta_ - b_cos_eta * owner_.sin_theta_;
      e_deriv_y = -a_sin_eta * owner_.sin_theta_ + b_cos_eta * owner_.cos_theta_;
    }
  };

private:
  const Number cx_, cy_, a_, b_;
  int size_;
  Number cos_theta_, sin_theta_;
  Number eta1_, alpha_, deta_;

  static Number rational_function(Number x, Number const c[4])
  {
    return ((c[0] * x + c[1]) * x + c[2]) / (x + c[3]);
  }

  Number error_estimation(Number eta1, Number eta2) const
  {
    typedef detail::arc_to_cubic_bezier_error_coefficients<Number> coefficients;

    Number b_div_a = b_/a_;
    Number cos_eta_sum[3];
    {
      Number eta_sum = eta1 + eta2;
      Number eta_sum_j = eta_sum;
      for(int j=0; j<3; ++j, eta_sum_j += eta_sum)
        cos_eta_sum[j] = std::cos(eta_sum_j);
    }
    bool coefficient_set = b_div_a >= 0.25 ? 1 : 0;
    Number c[2];
    for(int i=0; i<2; ++i)
    {
      c[i] = rational_function(b_div_a, coefficients::mu[coefficient_set][i][0]);
      for(int j=1; j<=3; ++j)
        c[i] += rational_function(b_div_a, coefficients::mu[coefficient_set][i][j])
          * cos_eta_sum[j - 1];
    }
    return rational_function(b_div_a, coefficients::safety)
      * a_ * std::exp(c[0] + c[1] * (eta2 - eta1));
  }

  void calculate_step(threshold_tag, Number eta2, Number threshold)
  {
    deta_ = eta2 - eta1_;
    size_ = 1;
    bool found = false;
    while (!found && size_ < 1024)
    {
      if (deta_ <= boost::math::constants::half_pi<Number>())
      {
        found = true;
        Number eta1b = eta1_;
        for(int i=0; i<size_; ++i)
        {
          Number eta1a = eta1b;
          eta1b += deta_;
          if (error_estimation(eta1a, eta1b) > threshold)
          {
            found = false;
            break;
          }
        }
      }
      size_ *= 2;
      deta_ *= 0.5;
    }
  }

  void calculate_step(max_angle_tag, Number eta2, Number max_angle)
  {
    deta_ = eta2 - eta1_;
    size_ = static_cast<int>(std::fabs(deta_) / max_angle + 1.0);
    deta_ /= size_;
  }

  void prepare(Number theta)
  {
    cos_theta_ = std::cos(theta);
    sin_theta_ = std::sin(theta);
    Number tan_deta = std::tan(deta_ * 0.5);
    alpha_ = std::sin(deta_) * (std::sqrt(4 + 3 * tan_deta * tan_deta) - 1) * boost::math::constants::third<Number>();
  }
};

}