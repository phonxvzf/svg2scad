#include "bezier.hpp"
#include "math/util.hpp"
#include "math/matrix.hpp"

namespace bezier {
  inline float angle_between(const vector2f& u, const vector2f& v) {
    return std::acos(u.dot(v) / (u.size_sq() * v.size_sq()));
  }

  vector2f curve(std::vector<vector2f> points, float t) {
    ASSERT(points.size() > 1);
    for (size_t step = 1; step <= points.size()-1; ++step) {
      for (size_t i = 0; i < points.size()-step; ++i) {
        points[i] = lerp(t, points[i], points[i+1]);
      }
    }
    return points.front();
  }

  vector2f elliptical_curve(
      const vector2f& p0,
      const vector2f& p1,
      const vector2f& r,
      bool large_arc_flag,
      bool sweep_flag,
      float x_axis_rotation,
      float t
      )
  {
    // convert endpoint parameterization to center parameterization
    // see https://www.w3.org/TR/SVG/implnote.html#ArcConversionEndpointToCenter
    static constexpr float two_pi = 6.28318530718f;
    const float neg     = (large_arc_flag == sweep_flag) ? -1.0f : 1.0f;
    const float sine    = std::sin(x_axis_rotation);
    const float cosine  = std::cos(x_axis_rotation);
    const float rx_sq   = pow2(r.x);
    const float ry_sq   = pow2(r.y);
    matrix2f rot_mat { { cosine, sine }, { -sine, cosine } };
    vector2f p0p = rot_mat.dot(p0 - p1) / 2.0f;

    const float x0p_sq = pow2(p0p.x);
    const float y0p_sq = pow2(p0p.y);
    const vector2f cp = neg * (
        1.f / std::sqrt(
          (rx_sq * y0p_sq + ry_sq * x0p_sq) / (rx_sq * ry_sq - rx_sq * y0p_sq - ry_sq * x0p_sq))
        ) * vector2f(r.x / r.y * y0p_sq, -r.y / r.x * x0p_sq);

    const vector2f c = rot_mat.t().dot(cp) + (p0 + p1) / 2.0f;
    const float theta0  = angle_between(
        { 1.0f, 0.0f }, { (p0p.x - cp.x) / r.x, (p0p.y - cp.y) / r.y }
        );
    float dtheta = std::fmod(angle_between(
        { (p0p.x - cp.x) / r.x, (p0p.y - cp.y) / r.y    },
        { (-p0p.x - cp.x) / r.x, (-p0p.y - cp.y) / r.y  }
        ), two_pi);

    if (sweep_flag && dtheta < 0.0f) dtheta += two_pi;
    else if (!sweep_flag && dtheta > 0.0f) dtheta -= two_pi;

    const float angle = theta0 + t * dtheta;
    return c + vector2f(r.x * std::cos(angle), r.y * std::sin(angle));
  }
}
