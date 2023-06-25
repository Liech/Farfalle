#pragma once

#include <vector>
#include <glm/glm.hpp>

class Mesh2D;

class LineSoup {
public:
  LineSoup(const Mesh2D&);
  std::vector<std::pair<glm::dvec2, glm::dvec2>> segmentCut(glm::dvec2 start, glm::dvec2 end);

private:
  
};