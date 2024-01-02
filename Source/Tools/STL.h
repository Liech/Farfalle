#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

class STL {
public:
  static void write(const std::string& filename, const std::vector<glm::dvec3>&);
  static std::vector<glm::dvec3> read(const std::string& filename);
};