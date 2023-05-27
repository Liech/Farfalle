#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "GCode/Printer.h"
#include "Tools/MovementType.h"

namespace GCode
{
  class Travel
  {
  public:
    Travel(Printer&);

    std::vector<glm::dvec3> controlPoints;

    void toString(std::string& result) const;

  private:
    Printer& printer;
  };
}