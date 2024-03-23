#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

class Printer;

namespace GCode
{
  class Travel
  {
  public:
    Travel(Printer&);
    virtual ~Travel();

    std::vector<glm::dvec3> controlPoints;

    void toString(std::string& result) const;

  private:
    Printer& printer;
  };
}