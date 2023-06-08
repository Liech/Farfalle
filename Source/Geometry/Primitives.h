#pragma once

#include <glm/glm.hpp>
#include "Model.h"

class Primitives {
public:
  static std::shared_ptr<Model> sphere(const glm::dvec3& position, double radius);
};