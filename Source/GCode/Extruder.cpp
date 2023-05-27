#include "Extruder.h"

#include "LinearPrint.h"
#include "Travel.h"
#include "Movement.h"

Extruder::Extruder(Printer& p ) : printer(p){

}

void Extruder::startup(std::string& result) {
  result += "\n; Extruder Startup: \n";
  result += "M201 E" + std::to_string(accelerationLimits) + " ; sets maximum accelerations\n";
  result += "M203 E" + std::to_string(maxFeedrate) + " ; sets maximum feedrates\n";
  result += "M205 E" + std::to_string(jerkLimit) + " ; jerk limits \n";
  result += "M221 S" + std::to_string(flow) + " ; set flow\n";
  result += "M900 K" + std::to_string(linearAdvance) + " ; linear advance\n";
  result += "M205 S" + std::to_string(minimumExtrudingFeedRate) + " ; extruding feedrate \n";
  result += "G83 ; relative extruder coords\n";
  result += "G21 ; units to millimeter\n";
  result += "G92 E0 ; set extruder pos to 0\n";
  result += "M221 S" + std::to_string(flow) + " ; set flow\n";
  result += "M900 K" + std::to_string(linearAdvance) + " ; linear advance\n";
}

void Extruder::prime(std::string& result) {
  GCode::Travel      travel   (printer);
  GCode::LinearPrint primeLine(printer);  

  result += "\n; Prime Nozzle: \n";

  travel.controlPoints = { glm::dvec3(printer.dimensions[0]/2,0,printer.movement->currentPosition[2]), glm::dvec3(printer.dimensions[0]/2,0,startPrintHeight)};

  glm::dvec3 primeEnd = glm::dvec3(0, 0, startPrintHeight);
  primeLine.controlPoints = { glm::dvec3(printer.dimensions[0]/2,0,startPrintHeight), primeEnd };
  primeLine.feedrate = 0.4;

  printer.movement->travelMode(result);
  travel.toString(result);

  printer.movement->printMode(result);
  primeLine.toString(result);
  printer.movement->currentPosition = primeEnd;
}

void Extruder::move(std::string& result, double offset) {
  std::string msg = "";
  if (offset < 0)
    msg = "Retract";
  else
    msg = "Unretract";

  result += "G1 E" + std::to_string(position + offset) + " ; "+ msg +"\n";
  position += offset;
}

void Extruder::retract(std::string& result) {
  printer.movement->retractMode(result);
  move(result, -2);
}

void Extruder::unretract(std::string& result) {
  printer.movement->retractMode(result);
  move(result, 2);
}