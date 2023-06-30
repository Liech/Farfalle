#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include  "Model.h"

class LineSoup {
public:
  LineSoup(const Model&);

  std::vector<std::pair<glm::dvec2, glm::dvec2>> segmentCut(glm::dvec2 start, glm::dvec2 end);
  void save(const std::string& filename);

private:
  void addToSoup(const std::pair<glm::dvec3, glm::dvec3>&);

  const Model& target;
  std::vector<std::pair<glm::dvec2, glm::dvec2>> soup;
};