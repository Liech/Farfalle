#pragma once

#include <glm/glm.hpp>
#include <memory>

class Model;

class Primitives {
public:
  static std::unique_ptr<Model> sphere(const glm::dvec3& position, double radius, int xyAmount = 20, int zAmount = 20);
};