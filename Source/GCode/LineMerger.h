#pragma once

#include <glm/glm.hpp>
#include <vector>

class LineMerger {
public:
  LineMerger();
  std::vector<std::vector<glm::dvec3>> merge(const std::vector<std::vector<glm::dvec3>>& input, double mergeDistance);
};