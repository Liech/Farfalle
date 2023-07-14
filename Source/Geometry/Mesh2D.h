#pragma once

#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

class Mesh2DPIMPL;
class Model;


class Mesh2D {
public:
  Mesh2D(const std::vector<std::vector<glm::dvec3>>&);
  Mesh2D(Model&, const std::vector<std::vector<glm::dvec3>>& loops);
  Mesh2D();

  std::vector<std::vector<glm::dvec2>> fill(double distance);

  std::shared_ptr<Mesh2D> difference(const Mesh2D& other); //bool operation
  std::vector<std::shared_ptr<Mesh2D>> decompose(); //poly w. holes to polys
  void savePoly(const std::string& filename) const;
private:
  std::vector<glm::dvec2> fill(double distance, int index);
  glm::dvec2 raycast(const glm::dvec2& start, const glm::dvec2& dir, int index, bool& doesIntersect);

  void init(const std::vector<std::vector<glm::dvec2>>&);

  std::shared_ptr<Mesh2DPIMPL> p;
};