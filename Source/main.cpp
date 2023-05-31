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

#include <CGAL/Simple_cartesian.h>

typedef CGAL::Simple_cartesian<double> Kernel; 
typedef Kernel::Point_2 Point_2;

int main() {
  Point_2 p(1, 1), q(10, 10);

  Printer printer;

  std::string data = "";
  data += "; Farfalle GCODE Generator\n";

  printer .startup       (data);

  data += "\n; ;;;;;;;;;;;;;;;;;;";
  data += "\n; Printing:  ";
  data += "\n; ;;;;;;;;;;;;;;;;;;\n";
  
  for (int i = 0; i < 20; i++) {
    data += "; " + std::to_string(i) + "\n";
    GCode::Travel      travel(printer);
    GCode::LinearPrint line(printer);

    glm::dvec2 startPos = glm::dvec2(i * 5, 190);
    glm::dvec2 endPos   = glm::dvec2(i * 5, 90 );
    travel.controlPoints = { printer.movement->currentPosition + glm::dvec3(0,0,1) ,glm::dvec3(startPos[0], startPos[1],printer.movement->currentPosition[2] + 1), glm::dvec3(startPos[0], startPos[1],0.2)};
    line  .controlPoints = { glm::dvec3(endPos[0],endPos[1],0.2)};
    line  .feedrate = 0.1 + 0.1*i;


    printer.extruder->retract(data);
    printer.movement->travelMode(data);
    travel.toString(data);
    printer.extruder->unretract(data);
    printer.movement->printMode(data);
    line.toString(data);
    printer.extruder->retract(data);
  }


  printer.shutdown(data);

  Tools::String2File("D:\\Farfalle\\firstTest.gcode",data);
  //Tools::String2File("firstTest.gcode", data);

  return 0;
}