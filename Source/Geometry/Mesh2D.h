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

  std::shared_ptr<Mesh2D> difference(const Mesh2D& other);
  void save(const std::string& filename);
private:
  void init(const std::vector<std::vector<glm::dvec2>>&);

  std::shared_ptr<Mesh2DPIMPL> p;
};