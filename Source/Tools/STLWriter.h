#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

class STLWriter {
public:
  static void write(const std::string& filename, const std::vector<glm::dvec3>&);
};