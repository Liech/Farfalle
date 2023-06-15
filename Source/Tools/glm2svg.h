#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Tools {
  class glm2svg {
  public:
    static void save(const std::string& filename, const std::vector<std::vector<glm::dvec2>>&, const std::vector<std::string> colors = {});
  };
}