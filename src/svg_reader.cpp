#include <iostream>

#include "svg_reader.hpp"

namespace svg {
  reader::context::~context() {
  }

  const std::vector<std::shared_ptr<action::base>>& reader::context::actions() const {
    return m_actions;
  }

  void reader::context::path_move_to(float x, float y, svgpp::tag::coordinate::absolute) {
    m_actions.push_back(std::make_shared<action::move_to>(action::move_to(vector2f(x, y))));
  }

  void reader::context::path_line_to(float x, float y, svgpp::tag::coordinate::absolute) {
    m_actions.push_back(std::make_shared<action::line_to>(action::line_to(vector2f(x, y))));
  }

  void reader::context::path_quadratic_bezier_to(
      float x1, float y1,
      float x, float y,
      svgpp::tag::coordinate::absolute)
  {
    m_actions.push_back(
        std::make_shared<action::quadratic_bezier_to>(
          action::quadratic_bezier_to({ x1, y1 }, { x, y })
          )
        );
  }

  void reader::context::path_cubic_bezier_to(
      float x1, float y1,
      float x2, float y2,
      float x, float y,
      svgpp::tag::coordinate::absolute)
  {
    m_actions.push_back(
        std::make_shared<action::cubic_bezier_to>(
          action::cubic_bezier_to(
            { x1, y1 },
            { x2, y2 },
            { x, y }
            )
          )
        );
  }

  void reader::context::path_elliptical_arc_to(
      float rx, float ry, float x_axis_rotation,
      bool large_arc_flag, bool sweep_flag,
      float x, float y,
      svgpp::tag::coordinate::absolute)
  {
    m_actions.push_back(
        std::make_shared<action::elliptic_arc_to>(
          action::elliptic_arc_to(
            { rx, ry },
            x_axis_rotation,
            { x, y },
            large_arc_flag,
            sweep_flag
            )
          )
        );
  }

  void reader::context::path_close_subpath() {
  }

  void reader::context::path_exit() {
  }

  void reader::context::on_enter_element(svgpp::tag::element::any) {

  }

  void reader::context::on_exit_element() {

  }

  reader::reader() {
  }

  reader::reader(const std::string& fpath) {
    this->load_file(fpath);
  }

  reader::~reader() {
  }

  const std::vector<std::shared_ptr<action::base>>& reader::load_file(const std::string& fpath) {
    rapidxml_ns::file<> svg_file(fpath.c_str());
    rapidxml_ns::xml_document<> svg_document;
    svg_document.parse<0>(svg_file.data());

    rapidxml_ns::xml_node<>* svg_element = svg_document.first_node("svg");
    rapidxml_ns::xml_attribute<>* attr_width = svg_element->first_attribute("width");
    rapidxml_ns::xml_attribute<>* attr_height = svg_element->first_attribute("height");
    if (attr_width) m_width = std::atof(attr_width->value());
    else std::cerr << "warning: could not determine base width" << std::endl;
    if (attr_height) m_height = std::atof(attr_height->value());
    else std::cerr << "warning: could not determine base height" << std::endl;

    if (svg_element) {
      svgpp::document_traversal<
        svgpp::processed_elements<processed_element_t>,
        svgpp::processed_attributes<svgpp::traits::shapes_attributes_by_element>
          >::load_document(svg_element, m_context);
    } else {
      throw std::runtime_error("svg tag not found (is this an SVG file?)");
    }

    return this->actions();
  }

  const std::vector<std::shared_ptr<action::base>>& reader::actions() const {
    return m_context.actions();
  }

  float reader::width() const {
    return m_width;
  }

  float reader::height() const {
    return m_height;
  }
} /* namespace svg */
