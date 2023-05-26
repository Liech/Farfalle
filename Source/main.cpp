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

  glm::dvec3 current = glm::dvec3(0, 0, 100);

  Printer printer;
  Temperature temp;

  GCode::LinearSpline travel(printer);
  GCode::LinearSpline line(printer);

  line.controlPoints = { glm::dvec3(60,58.7,0.2)};
  line.feedrate = 0.0267225;

  travel.controlPoints = { glm::dvec3(60,61.3,current[2]), glm::dvec3(60,61.3,0.2)};
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

  Tools::String2File("result.gcode",data);

  return 0;
}