#include "LinearPrint.h"

#include "Extruder.h"
#include "Movement.h"

namespace GCode
{
  LinearPrint::LinearPrint(Printer& _printer) : printer(_printer){

  }

  void LinearPrint::toString(std::string& result) const
  {
    double f = feedrate;
    if (f >= printer.extruder->maximumFeedrate) {
      double max = printer.extruder->maximumFeedrate;
      double factor = f / max;
      //f = max;
      printer.movement->customMode(result, printer.movement->printSpeed / factor);
    }
    else
      printer.movement->printMode(result);


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
      if (f != 0) {
        printer.extruder->position += f * distance;
        result += " E" + std::to_string(printer.extruder->position);
      }

      result += " ; Print ";
      result+= "\n";
    }
  }
}