#ifndef BEZIER_HPP
#define BEZIER_HPP

#include <vector>
#include "math/vector.hpp"

namespace bezier {
  using namespace math;

  vector2f curve(std::vector<vector2f> points, float t);

  vector2f elliptical_curve(
      const vector2f& p0,
      const vector2f& p1,
      const vector2f& r,
      bool large_arc_flag,
      bool sweep_flag,
      float x_axis_rotation,
      float t
      );

  inline vector2f quadratic_curve(
      const vector2f& p0,
      const vector2f& p1,
      const vector2f& p2,
      float t)
  {
    return curve({ p0, p1, p2 }, t);
  }

  inline vector2f cubic_curve(
      const vector2f& p0,
      const vector2f& p1,
      const vector2f& p2,
      const vector2f& p3,
      float t)
  {
    return curve({ p0, p1, p2, p3 }, t);
  }
}

#endif /* BEZIER_HPP */
