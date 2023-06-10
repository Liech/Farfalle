#pragma once

#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

class Mesh2DPIMPL;

class Mesh2D {
public:
  Mesh2D(const std::vector<std::vector<glm::dvec3>>&);

  void save(const std::string& filename);
private:
  std::shared_ptr<Mesh2DPIMPL> p;
};