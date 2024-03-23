#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Tools/MovementType.h"

class Printer;

namespace GCode
{
  class LinearPrint
  {
  public:
    LinearPrint(Printer&);
    virtual ~LinearPrint();

    std::vector<glm::dvec3> controlPoints;    
    double feedrate = 0.0267225;

    void toString(std::string& result) const;

  private:
    Printer& printer;
  };
}