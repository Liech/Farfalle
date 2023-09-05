#pragma once

#include <glm/glm.hpp>
#include <vector>

class LineSimplification {
public:
  static std::vector<glm::dvec3> simplify(const std::vector<glm::dvec3>&, double tolerance = 0.01f);
};