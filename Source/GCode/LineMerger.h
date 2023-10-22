#pragma once

#include <glm/glm.hpp>
#include <vector>

class LineMerger {
public:
  LineMerger();
  static std::vector<std::vector<glm::dvec3>> merge(const std::vector<std::vector<glm::dvec3>>& input, double mergeDistance);

private:
  static double getLength(const std::vector<glm::dvec3>&);
};