#pragma once

#include <vector>
#include <glm/glm.hpp>

class LineTracer {
public:
  static std::vector<std::vector<glm::ivec3>> traceLines(const bool* data, const glm::ivec3& resolution); 
  static std::vector<glm::ivec3> gatherTrues(const bool* data, const glm::ivec3& resolution);

private:
  static constexpr std::vector<glm::ivec3> getNeighbours();
  static void removePointsOnLine(std::vector<glm::ivec3>&);
};