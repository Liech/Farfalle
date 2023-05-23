#include <iostream>

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include "Tools/String2File.h"

#include "GCode/Startup.h"
#include "GCode/Shutdown.h"
#include "GCode/LinearSpline.h"

int main() {
  glm::dvec3 x;
  nlohmann::json j;
  j["x"] = "x";


  GCode::Startup start;
  GCode::Shutdown end;
  GCode::LinearSpline line;

  std::string data = start.toString();
  data += line.toString();
  data += end.toString();

  Tools::String2File("result.gcode",data);

  return 0;
}