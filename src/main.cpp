#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

#include "svg_reader.hpp"
#include "bezier.hpp"

const std::string SCAD_DISCLAIMER =
"/*\n"
" * This file is generated with svg2scad.\n"
" * Direct modification to its contents is discouraged.\n"
" */\n";

const std::string SCAD_PREAMBLE = "function svg_distance(p0,p1)=sqrt((p0.x-p1.x)*(p0.x-p1.x)+(p0.y-p1.y)*(p0.y-p1.y));function svg_normalize(v)=v/svg_distance(v,[0,0]);";

const std::string SCAD_MODULE_DRAW_LINE = "module svg_line(p0,p1,thickness,depth){size=svg_distance(p0,p1);angle=atan2(p1.y-p0.y,p1.x-p0.x);translate(p0){rotate(angle,[0,0,1]){translate([0,-0.5*thickness,0]){scale([size,thickness,depth]){cube([1,1,1]);}}}}}";

const std::string SCAD_MODULE_DRAW_VERTICES = "module svg_curve(vertices,thickness,depth){extent=0.5*thickness;n_segments=len(vertices)-1;union(){for(i=[0:n_segments-1]){p=vertices[max(0,i-1)];p0=vertices[i];p1=vertices[i+1];tangent0=p0-p;tangent1=p1-p0;normal0=svg_normalize([tangent0.y,-tangent0.x]);normal1=svg_normalize([tangent1.y,-tangent1.x]);fpr0=p0+extent*normal0;fpr1=p0+extent*normal1;fpl0=p0-extent*normal0;fpl1=p0-extent*normal1;svg_line(p0,p1,thickness,depth);color([1,0,0])translate([0,0,0.5*depth])scale([1,1,depth])polygon([p0,fpr0,fpr1]);color([0,1,0])translate([0,0,0.5*depth])scale([1,1,depth])polygon([p0,fpl0,fpl1]);}}}";

using namespace math;

void scad_print_header() {
  std::cout << SCAD_DISCLAIMER << std::endl;
  std::cout << SCAD_PREAMBLE << std::endl;
  std::cout << SCAD_MODULE_DRAW_LINE << std::endl;
  std::cout << SCAD_MODULE_DRAW_VERTICES << std::endl;
}

std::string scad_vertices_list(const std::vector<vector2f>& vertices) {
  std::stringstream ss;
  ss << "[";
  for (size_t i = 0; i < vertices.size(); ++i) {
    ss << "[" << vertices[i].x << "," << vertices[i].y << "]";
    if ((i + 1) < vertices.size()) {
      ss << ",";
    }
  }
  ss << "]";
  return ss.str();
}

void scad_print_line(const vector2f& from, const vector2f& to) {
  std::cout << "svg_line([" << from.x << "," << from.y << "],"
    << "[" << to.x << "," << to.y << "],thickness,depth);" << std::endl;
}

void scad_print_curve(const std::vector<vector2f>& vertices) {
  if (vertices.empty()) return;
  std::cout << "svg_curve(" << scad_vertices_list(vertices) << ",thickness,depth);" << std::endl;
}

struct scad_vertex {
  vector2f vertex;
  bool moveflag = false;
};

void print_help_and_exit() {
  std::cerr
    <<
    "Usage: svg2scad [OPTIONS] SVGFILE\n\n"
    "Available Options:\n"
    "\t-o, --output\tSpecify OpenSCAD output file name with extension\n"
    "\t-m, --modname\tSpecify module name to be generated. `svg_generated' will be used if"
    " this option is not specified.\n"
    "\t-s, --segments\tSpecify number of line segments for each curve\n"
    "\t-h, --help\tPrint this help text and exit with failure\n"
    << std::endl;
  std::exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    print_help_and_exit();
  }

  size_t n_segments = 10;
  std::string output_fpath("");
  std::string module_name("svg_generated");
  std::string svg_fpath("");
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output")) {
      if (i + 1 >= argc) print_help_and_exit();
      output_fpath = argv[i+1];
      ++i;
    } else if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--modname")) {
      if (i + 1 >= argc) print_help_and_exit();
      module_name = argv[i+1];
      ++i;
    } else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--segments")) {
      if (i + 1 >= argc) print_help_and_exit();
      n_segments = atoi(argv[i+1]);
      ++i;
    } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
      print_help_and_exit();
    } else {
      svg_fpath = argv[i];
    }
  }

  n_segments = std::max(1ul, n_segments);

  svg::reader svg_reader;
  try {
    svg_reader.load_file(svg_fpath);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  if (!output_fpath.empty()) {
    std::freopen(output_fpath.c_str(), "w", stdout);
  }

  vector2f marker(0, 0);
  std::vector<scad_vertex> scad_vertices;
  scad_print_header();

  for (const auto& action : svg_reader.actions()) {
    auto action_type = svg::action::get_type(action);
    switch (action_type) {
      case svg::action::ACTION_MOVE_TO:
        {
          const auto& move_to = std::dynamic_pointer_cast<const svg::action::move_to>(action);
          marker = move_to->dst;
          scad_vertices.push_back({ marker, true });
          break;
        }
      case svg::action::ACTION_LINE_TO:
        {
          const auto& line_to = std::dynamic_pointer_cast<const svg::action::line_to>(action);
          marker = line_to->dst;
          scad_vertices.push_back({ marker, false });
          break;
        }
      case svg::action::ACTION_QUAD_BEZIER_TO:
        {
          const auto& quad_to
            = std::dynamic_pointer_cast<const svg::action::quadratic_bezier_to>(action);
          for (size_t i = 1; i <= n_segments; ++i) {
            float t = (float) i / n_segments;
            scad_vertices.push_back(
                { bezier::quadratic_curve(marker, quad_to->p1, quad_to->p2, t), false }
                );
          }
          marker = quad_to->dst;
          break;
        }
      case svg::action::ACTION_CUBIC_BEZIER_TO:
        {
          const auto& cubic_to
            = std::dynamic_pointer_cast<const svg::action::cubic_bezier_to>(action);
          for (size_t i = 1; i <= n_segments; ++i) {
            float t = (float) i / n_segments;
            scad_vertices.push_back(
                { bezier::cubic_curve(marker, cubic_to->p1, cubic_to->p2, cubic_to->p3, t), false }
                );
          }
          marker = cubic_to->dst;
          break;
        }
      case svg::action::ACTION_ELLIPTIC_ARC_TO:
        {
          const auto& arc_to
            = std::dynamic_pointer_cast<const svg::action::elliptic_arc_to>(action);
          for (size_t i = 1; i <= n_segments; ++i) {
            float t = (float) i / n_segments;
            scad_vertices.push_back(
                { bezier::elliptical_curve(
                    marker,
                    arc_to->p1,
                    arc_to->r,
                    arc_to->large_arc,
                    arc_to->sweep,
                    arc_to->x_axis_rotation,
                    t
                    ), false }
                );
          }
          marker = arc_to->dst;
          break;
        }
      default:
        std::cerr << "error: unknown SVG action" << std::endl;
        return EXIT_FAILURE;
    }
  } /* for action */

  std::cout << "module " << module_name << "(thickness=1,depth=1) {" << std::endl;

  std::vector<vector2f> curve_vertices;
  for (size_t i = 0; i < scad_vertices.size(); ++i) {
    while (i < scad_vertices.size() && !scad_vertices[i].moveflag) {
      curve_vertices.push_back(scad_vertices[i++].vertex);
    }
    scad_print_curve(curve_vertices);
    curve_vertices.clear();
  }

  std::cout << "}" << std::endl;

  return EXIT_SUCCESS;
}
