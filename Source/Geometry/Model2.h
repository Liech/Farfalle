#pragma once

#include <vector>
#include <glm/glm.hpp>

//cgal free & faster implementation of Model
class Model2 {
public:
  std::vector<glm::dvec3> vertices;
  std::vector<size_t>     indices ;
private:
};