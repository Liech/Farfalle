#include <iostream>

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include "Tools/String2File.h"

#include "GCode/LinearSpline.h"
#include "Config/Printer.h"
#include "Config/Temperature.h"

int main() {
  glm::dvec3 x;
  nlohmann::json j;
  j["x"] = "x";

  Printer printer;
  Temperature temp;

  GCode::LinearSpline travel(printer);
  GCode::LinearSpline line(printer);

  line.controlPoints = { glm::dvec3(80,58.7,0.2)};
  line.feedrate = 0.0267225;

  travel.controlPoints = { glm::dvec3(60,61.3,printer.currentPosition[2]), glm::dvec3(60,61.3,0.2)};
  travel.feedrate = 0;

  std::string data = "";
  temp   .fanOff        (data);
  temp   .startHeating  (data);
  printer.startup       (data);
  printer.home          (data);
  temp   .waitForHeating(data);

  travel.toString(data);
  line.toString(data);

  temp   .cooldown(data);
  printer.shutdown(data);

  Tools::String2File("E:\\Farfalle\\firstTest.gcode",data);

  return 0;
}