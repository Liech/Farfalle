#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace GCode
{
  class LinearSpline
  {
  public:
    std::vector<glm::dvec3> controlPoints;    
    double feedrate = 0.0267225;

    std::string toString() const;
  };
}