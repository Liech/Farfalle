#include "LinearSpline.h"

#include "Extruder.h"
#include "Movement.h"

namespace GCode
{
  LinearSpline::LinearSpline(Printer& _printer) : printer(_printer){

  }

  void LinearSpline::toString(std::string& result) const
  {
    //result += "; Linear Spline: \n";
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
      if (feedrate != 0) {
        printer.extruder->position += feedrate * distance;
        result += " E" + std::to_string(printer.extruder->position);
      }


      result += " ; Linear Movement ";
      if (feedrate != 0)
        result += "with Extrusion";
      result+= "\n";
    }
  }
}