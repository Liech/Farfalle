#include "LinearSpline.h"

namespace GCode
{
  LinearSpline::LinearSpline(Printer& _printer) : printer(_printer){

  }

  void LinearSpline::toString(std::string& result) const
  {
    result += "\n; Linear Spline: \n";
    for (auto& point : controlPoints)
    {
      double distance = glm::distance(point, printer.currentPosition);
      if (distance == 0)
        continue;

      result += "G1";
      if (point[0] != printer.currentPosition[0]) {
        result += " X" + std::to_string(point[0]);
        printer.currentPosition[0] = point[0];
      }
      if (point[1] != printer.currentPosition[1]) {
        result += " Y" + std::to_string(point[1]);
        printer.currentPosition[1] = point[1];
      }
      if (point[2] != printer.currentPosition[2]) {
        result += " Z" + std::to_string(point[2]);
        printer.currentPosition[2] = point[2];
      }
      if (feedrate != 0)
        result += " E" + std::to_string(feedrate * distance);
      result += " ; Linear Movement ";
      if (feedrate != 0)
        result += "with Extrusion";
      result+= "\n";
    }
  }
}