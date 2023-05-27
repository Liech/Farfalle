#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "GCode/Printer.h"
#include "Tools/MovementType.h"

namespace GCode
{
  class LinearSpline
  {
  public:
    LinearSpline(Printer&);

    std::vector<glm::dvec3> controlPoints;    
    double feedrate = 0.0267225;

    void toString(std::string& result) const;

  private:
    Printer& printer;
  };
}