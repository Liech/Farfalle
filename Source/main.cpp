#include <iostream>

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

int main() {
  glm::dvec3 x;
  nlohmann::json j;
  j["x"] = "x";
  return 0;
}