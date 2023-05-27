#include <iostream>

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include "Tools/String2File.h"

#include "GCode/Travel.h"
#include "GCode/LinearPrint.h"
#include "GCode/Printer.h"
#include "GCode/Temperature.h"
#include "GCode/Extruder.h"
#include "GCode/Movement.h"

int main() {
  glm::dvec3 x;
  nlohmann::json j;
  j["x"] = "x";

  Printer printer;

  GCode::Travel      travel(printer);
  GCode::LinearPrint line  (printer);

  line.controlPoints = { glm::dvec3(10,10,0.2)};
  line.feedrate = 0.16;

  travel.controlPoints = { printer.movement->currentPosition+glm::dvec3(0,0,1) ,glm::dvec3(190, 190,printer.movement->currentPosition[2]+1), glm::dvec3(190,190,0.2)};

  std::string data = "";
  data += "; Farfalle GCODE Generator\n";

  printer .startup       (data);

  data += "\n; ;;;;;;;;;;;;;;;;;;";
  data += "\n; Printing:  ";
  data += "\n; ;;;;;;;;;;;;;;;;;;\n";
  printer.extruder->retract(data);
  printer.movement->travelMode(data);
  travel.toString(data);
  printer.extruder->unretract(data);
  printer.movement->printMode(data);
  line  .toString(data);
  printer.extruder->retract(data);

  printer.shutdown(data);

  Tools::String2File("D:\\Farfalle\\firstTest.gcode",data);
  //Tools::String2File("firstTest.gcode", data);

  return 0;
}