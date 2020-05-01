#ifndef SVG_READER_HPP
#define SVG_READER_HPP

#include <string>
#include <memory>

#include "svgpp/svgpp.hpp"
#include "rapidxml_ns/rapidxml_ns_utils.hpp"
#include "svgpp/policy/xml/rapidxml_ns.hpp"
#include "math/util.hpp"

namespace svg {

  using namespace math;

  namespace action {
    enum type {
      ACTION_MOVE_TO,
      ACTION_LINE_TO,
      ACTION_QUAD_BEZIER_TO,
      ACTION_CUBIC_BEZIER_TO,
      ACTION_ELLIPTIC_ARC_TO,
      ACTION_UNKNOWN
    };

    struct base {
      virtual ~base() {}
    };

    struct move_to : base {
      union { vector2f p1, dst; };

      move_to() = delete;
      move_to(const vector2f& _p1) : p1(_p1) {}
    };

    struct line_to : base {
      union { vector2f p1, dst; };

      line_to() = delete;
      line_to(const vector2f& _p1) : p1(_p1) {}
    };

    struct quadratic_bezier_to : base {
      vector2f p1;
      union { vector2f p2, dst; };

      quadratic_bezier_to() = delete;
      quadratic_bezier_to(const vector2f& _p1, const vector2f& _p2) : p1(_p1), p2(_p2) {}
    };

    struct cubic_bezier_to : base {
      vector2f p1, p2;
      union { vector2f p3, dst; };

      cubic_bezier_to() = delete;
      cubic_bezier_to(const vector2f& _p1, const vector2f& _p2, const vector2f& _p3)
        : p1(_p1), p2(_p2), p3(_p3) {}
    };

    struct elliptic_arc_to : base {
      vector2f r;
      float x_axis_rotation;
      union { vector2f p1, dst; }; 
      bool large_arc;
      bool sweep;

      elliptic_arc_to() = delete;
      elliptic_arc_to(
          const vector2f& _r,
          float _x_axis_rotation,
          const vector2f& _p1,
          bool _large_arc,
          bool _sweep)
        : r(_r), x_axis_rotation(_x_axis_rotation), p1(_p1), large_arc(_large_arc), sweep(_sweep) {}
    };

    inline type get_type(const std::shared_ptr<base>& action) {
      base* base_ptr = action.get();
      if (dynamic_cast<move_to*>(base_ptr))             return ACTION_MOVE_TO;
      if (dynamic_cast<line_to*>(base_ptr))             return ACTION_LINE_TO;
      if (dynamic_cast<quadratic_bezier_to*>(base_ptr)) return ACTION_QUAD_BEZIER_TO;
      if (dynamic_cast<cubic_bezier_to*>(base_ptr))     return ACTION_CUBIC_BEZIER_TO;
      if (dynamic_cast<elliptic_arc_to*>(base_ptr))     return ACTION_ELLIPTIC_ARC_TO;
      return ACTION_UNKNOWN;
    }
  } /* namespace action */

  class reader {
    private:
      class context {
        private:
          std::vector<std::shared_ptr<action::base>> m_actions;

        public:
          context() = default;
          ~context();

          const std::vector<std::shared_ptr<action::base>>& actions() const;

          // SVG events
          void path_move_to(float x, float y, svgpp::tag::coordinate::absolute);
          void path_line_to(float x, float y, svgpp::tag::coordinate::absolute);
          void path_quadratic_bezier_to(
              float x1, float y1,
              float x, float y,
              svgpp::tag::coordinate::absolute);
          void path_cubic_bezier_to(
              float x1, float y1,
              float x2, float y2,
              float x, float y,
              svgpp::tag::coordinate::absolute);
          void path_elliptical_arc_to(
              float rx, float ry, float x_axis_rotation,
              bool large_arc_flag, bool sweep_flag,
              float x, float y,
              svgpp::tag::coordinate::absolute);
          void path_close_subpath();
          void path_exit();

          // XML events
          void on_enter_element(svgpp::tag::element::any);
          void on_exit_element();

          static const bool convert_only_rounded_rect_to_path = false;
      } m_context;

      float m_width   = 0.0f;
      float m_height  = 0.0f;

    public:
      reader();
      reader(const std::string& fpath);
      ~reader();

      const std::vector<std::shared_ptr<action::base>>& actions() const;
      const std::vector<std::shared_ptr<action::base>>& load_file(const std::string& fpath);
      float width() const;
      float height() const;
  }; /* class reader */

  using processed_element_t = boost::mpl::set<
    svgpp::tag::element::svg,
    svgpp::tag::element::g,
    svgpp::tag::element::circle,
    svgpp::tag::element::ellipse,
    svgpp::tag::element::line,
    svgpp::tag::element::path,
    svgpp::tag::element::polygon,
    svgpp::tag::element::polyline,
    svgpp::tag::element::rect
      >;
} /* namespace svg */

#endif /* SVG_READER_HPP */
