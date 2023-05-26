#include "Printer.h"

#include <algorithm>

void Printer::startup(std::string& result) const
{
  result += "M201 X" + std::to_string(accelerationLimits[0]) + " Y"+ std::to_string(accelerationLimits[1]) + " Z" + std::to_string(accelerationLimits[2]) + " E" + std::to_string(extruderAccelerationLimits) + " ; sets maximum accelerations\n";
  result += "M203 X" + std::to_string(maxFeedrate[0]) + " Y" + std::to_string(maxFeedrate[1]) + " Z" + std::to_string(maxFeedrate[2]) + " E" + std::to_string(maxExtruderFeedrate) + " ; sets maximum feedrates\n";
  result += "M204 P" + std::to_string(printAcceleration) + " R" + std::to_string(retractAcceleration) + " T" + std::to_string(travelAcceleration) + " ; set acceleration for print, retract and travel \n";
  result += "M205 X" + std::to_string(jerkLimit[0]) + " R" + std::to_string(jerkLimit[1]) + " T" + std::to_string(jerkLimit[2]) + " E" + std::to_string(extruderJerkLimit) + " ; jerk limits \n";
  result += "M205 S" + std::to_string(minimumExtrudingFeedRate) + " T" + std::to_string(minimumTravelFeedRate)  + " ; extruding & travel feedrate \n";
  result += "G90 ; absolute coords\n";
  result += "G83 ; relative extruder coords\n";
  result += "G21 ; units to millimeter\n";
  result += "G90 ; absolute coords\n";
  result += "G83 ; relative distance for extrusion\n";
  result += "G92 E0 ; set extruder pos to 0\n";
  result += "M221 " + std::to_string(flow) + " ; set flow\n";
  result += "M900 K" + std::to_string(linearAdvance) + " ; linear advance\n";
}

void Printer::shutdown(std::string& result)
{
  result += "M84 ; disable motor\n";
  double z = std::clamp(currentPosition[2] - 5, 0.0, dimensions[2]);
  result += "G1 X" + std::to_string(restPosition[0]) + " Y" + std::to_string(restPosition[1]) + " Z" + std::to_string(z) + " ; go to home position\n";
  currentPosition = glm::dvec3(restPosition[0], restPosition[1], z);
}

void Printer::home(std::string& result) const
{
  result += "G28 ; Home\n";
  if (hasMeshBedLeveling)
    result += "G29 ; Mesh bed Level\n";
  result += "G1 X" + std::to_string(currentPosition[0]) + " Y" + std::to_string(currentPosition[1]) + " Z" + std::to_string(currentPosition[2]) + " ; go to home position\n";
}