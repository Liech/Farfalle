#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

class Model;

class LineSoup {
public:
  LineSoup(const Model&);

  std::vector<std::pair<glm::dvec2, glm::dvec2>> segmentCut(glm::dvec2 start, glm::dvec2 end);
  void save(const std::string& filename);

private:
  
  std::vector<std::pair<glm::dvec2, glm::dvec2>> soup;
};