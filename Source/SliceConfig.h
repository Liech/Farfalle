#pragma once

#include <glm/glm.hpp>

class SliceConfig {
public:

  //Implicit Surface
  glm::dvec3 CenterPoint = glm::dvec3(0,0,0);
  double     BoundingSphereRadius = 100;
  double     Precision = 1;
  double     layerWidth = 0.4;
};