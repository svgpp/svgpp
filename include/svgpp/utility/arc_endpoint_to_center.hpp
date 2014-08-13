// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <algorithm>
#include <cmath>

namespace svgpp
{

template<class Number>
void arc_endpoint_to_center(Number x1, Number y1, Number x2, Number y2,
  Number & rx, Number & ry, Number phi, bool large_arc_flag, bool sweep_flag,
  Number & cx, Number & cy, Number & theta1, Number & theta2)
{
  Number sin_phi = std::sin(phi);
  Number cos_phi = std::cos(phi);

  Number x1s, y1s;
  {
    // SVG 1.1 (F.6.5.1)
    Number dx2 = (x1 - x2) * 0.5;
    Number dy2 = (y1 - y2) * 0.5;

    x1s =  cos_phi * dx2 + sin_phi * dy2;
    y1s = -sin_phi * dx2 + cos_phi * dy2;
  }
  Number cxs, cys;
  {
    
    Number rx2 = rx * rx;
    Number ry2 = ry * ry;
    Number x1s2 = x1s * x1s;
    Number y1s2 = y1s * y1s;
    // F.6.6 Correction of out-of-range radii
    Number lambda = x1s2 / rx2 + y1s2 / ry2; // (F.6.6.2)
    if (lambda > 1)
    {
      Number sqrt_lambda = std::sqrt(lambda);
      rx *= sqrt_lambda; 
      ry *= sqrt_lambda;
      rx2 = rx * rx;
      ry2 = ry * ry;
    }

    // SVG 1.1 (F.6.5.2)
    Number coeff = std::sqrt(std::max<Number>(0, (rx2 * ry2 - rx2 * y1s2 - ry2 * x1s2)/(rx2 * y1s2 + ry2 * x1s2)));
    if (large_arc_flag == sweep_flag)
      coeff = -coeff;
    Number rx_ry = rx / ry;
    cxs = coeff * rx_ry * y1s;
    cys = -coeff * x1s / rx_ry;
  }
  // SVG 1.1 (F.6.5.3)
  cx = cxs * cos_phi - cys * sin_phi + (x1 + x2) * 0.5;
  cy = cxs * sin_phi + cys * cos_phi + (y1 + y2) * 0.5;
  theta1 = std::atan2(( y1s - cys)/ry, ( x1s - cxs)/rx);
  theta2 = std::atan2((-y1s - cys)/ry, (-x1s - cxs)/rx);
}

}