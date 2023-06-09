#pragma once

#include <glm/glm.hpp>
#include "Model.h"

#include "CGALDefs.h"

class Primitives {
public:
  static std::shared_ptr<Nef_polyhedron> sphere(const glm::dvec3& position, double radius, int xyAmount = 20, int zAmount = 20);
};