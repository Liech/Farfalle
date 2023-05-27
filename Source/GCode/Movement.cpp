#include "Movement.h"

#include <algorithm>

Movement::Movement(Printer& p) : printer(p) {
  currentPosition = glm::dvec3(printer.dimensions[0] / 2  , printer.dimensions[1] / 2, 15);
  restPosition    = glm::dvec2(printer.dimensions[0] * 0.8, printer.dimensions[1] * 0.8);
}

void Movement::startup(std::string& result) {
  result += "\n; Movment Startup: \n";
  result += "M201 X" + std::to_string(accelerationLimits[0]) + " Y" + std::to_string(accelerationLimits[1]) + " Z" + std::to_string(accelerationLimits[2]) + " ; sets maximum accelerations\n";
  result += "M203 X" + std::to_string(maxFeedrate[0]) + " Y" + std::to_string(maxFeedrate[1]) + " Z" + std::to_string(maxFeedrate[2]) + " ; sets maximum feedrates\n";
  result += "M204 P" + std::to_string(printAcceleration) + " R" + std::to_string(retractAcceleration) + " T" + std::to_string(travelAcceleration) + " ; set acceleration for print, retract and travel \n";
  result += "M205 X" + std::to_string(jerkLimit[0]) + " R" + std::to_string(jerkLimit[1]) + " T" + std::to_string(jerkLimit[2]) + " ; jerk limits \n";
  result += "M205 T" + std::to_string(minimumTravelFeedRate) + " ; travel feedrate \n";

  result += "G90 ; absolute coords\n";
  result += "G21 ; units to millimeter\n";
  travelMode(result);
}

void Movement::home(std::string& result)
{
  result += "\n; Printer Home: \n";
  result += "G28 ; Home\n";
  if (hasMeshBedLeveling)
    result += "G29 ; Mesh bed Level\n";
  moveTo(result, currentPosition);
}

void Movement::moveTo(std::string& result, const glm::dvec3& pos)
{
  travelMode(result);
  result += "G1 X" + std::to_string(pos[0]) + " Y" + std::to_string(pos[1]) + " Z" + std::to_string(pos[2]) + "; travel\n";
  currentPosition = pos;
}

void Movement::shutdown(std::string& result) {
  result += "\n; Movement Shutdown: \n";
  double z = std::clamp(currentPosition[2] + 5, 0.0, printer.dimensions[2]);
  moveTo(result, glm::dvec3(currentPosition[0], currentPosition[1], z));
  moveTo(result, glm::dvec3(restPosition   [0], restPosition   [1], z));
  result += "M84 ; disable motor\n";
}

void Movement::travelMode(std::string& result) {
  if (travelType == MovementType::Travel)
    return;
  travelType = MovementType::Travel;
  result += "G1 F" + std::to_string(travelSpeed) + " ; travel Mode\n";
}

void Movement::printMode(std::string& result) {
  if (travelType == MovementType::Print)
    return;
  travelType = MovementType::Print;
  result += "G1 F" + std::to_string(printSpeed) + " ; print Mode\n";
}

void Movement::retractMode(std::string& result) {
  if (travelType == MovementType::Retract)
    return;
  travelType = MovementType::Retract;
  result += "G1 F" + std::to_string(retractSpeed) + " ; retract Mode\n";
}