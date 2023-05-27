#include "Travel.h"

#include "Extruder.h"
#include "Movement.h"

namespace GCode
{
  Travel::Travel(Printer& _printer) : printer(_printer){

  }

  void Travel::toString(std::string& result) const
  {
    printer.movement->travelMode(result);


    for (auto& point : controlPoints)
    {
      double distance = glm::distance(point, printer.movement->currentPosition);
      if (distance == 0)
        continue;

      result += "G1";
      if (point[0] != printer.movement->currentPosition[0]) {
        result += " X" + std::to_string(point[0]);
        printer.movement->currentPosition[0] = point[0];
      }
      if (point[1] != printer.movement->currentPosition[1]) {
        result += " Y" + std::to_string(point[1]);
        printer.movement->currentPosition[1] = point[1];
      }
      if (point[2] != printer.movement->currentPosition[2]) {
        result += " Z" + std::to_string(point[2]);
        printer.movement->currentPosition[2] = point[2];
      }

      result += " ; Travel ";
      result+= "\n";
    }
  }
}