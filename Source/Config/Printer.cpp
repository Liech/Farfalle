#include "Printer.h"

#include <algorithm>

void Printer::startup(std::string& result)
{
  result += "\n; Printer Startup: \n";
  result += "M201 X" + std::to_string(accelerationLimits[0]) + " Y"+ std::to_string(accelerationLimits[1]) + " Z" + std::to_string(accelerationLimits[2]) + " E" + std::to_string(extruderAccelerationLimits) + " ; sets maximum accelerations\n";
  result += "M203 X" + std::to_string(maxFeedrate[0]) + " Y" + std::to_string(maxFeedrate[1]) + " Z" + std::to_string(maxFeedrate[2]) + " E" + std::to_string(maxExtruderFeedrate) + " ; sets maximum feedrates\n";
  result += "M204 P" + std::to_string(printAcceleration) + " R" + std::to_string(retractAcceleration) + " T" + std::to_string(travelAcceleration) + " ; set acceleration for print, retract and travel \n";
  result += "M205 X" + std::to_string(jerkLimit[0]) + " R" + std::to_string(jerkLimit[1]) + " T" + std::to_string(jerkLimit[2]) + " E" + std::to_string(extruderJerkLimit) + " ; jerk limits \n";
  result += "M205 S" + std::to_string(minimumExtrudingFeedRate) + " T" + std::to_string(minimumTravelFeedRate)  + " ; extruding & travel feedrate \n";
  result += "G90 ; absolute coords\n";
  result += "G83 ; relative extruder coords\n";
  result += "G21 ; units to millimeter\n";
  result += "G92 E0 ; set extruder pos to 0\n";
  result += "M221 S" + std::to_string(flow) + " ; set flow\n";
  result += "M900 K" + std::to_string(linearAdvance) + " ; linear advance\n";
}

void Printer::shutdown(std::string& result)
{
  result += "\n; Printer Shutdown: \n";
  double z = std::clamp(currentPosition[2] + 5, 0.0, dimensions[2]);
  moveTo(result,glm::dvec3(restPosition[0], restPosition[1], z),0);
  result += "M84 ; disable motor\n";
}

void Printer::home(std::string& result)
{
  result += "\n; Printer Home: \n";
  result += "G28 ; Home\n";
  if (hasMeshBedLeveling)
    result += "G29 ; Mesh bed Level\n";
  moveTo(result, currentPosition, 0);
}

void Printer::moveTo(std::string& result, const glm::dvec3& pos, double feedrate)
{
  result += "G1 X" + std::to_string(pos[0]) + " Y" + std::to_string(pos[1]) + " Z" + std::to_string(pos[2]) + " E" + std::to_string(feedrate) + " ; go to home position\n";
  currentPosition = pos;
}