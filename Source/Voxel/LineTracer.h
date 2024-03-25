#pragma once

#include <vector>
#include <glm/glm.hpp>

class LineTracer {
public:
  static std::vector<std::vector<glm::dvec3>> traceLines(const bool* data, const glm::ivec3& resolution); 
  static std::vector<glm::ivec3> gatherTrues(const bool* data, const glm::ivec3& resolution);
};