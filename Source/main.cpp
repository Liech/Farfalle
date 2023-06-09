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

#include "Geometry/Geometry.h"
#include "Geometry/Model.h"


int main() {
  
  std::string filename = "C:\\Users\\nicol\\Downloads\\_3DBenchy_-_The_jolly_3D_printing_torture-test_by_CreativeTools_se_763622\\files\\3DBenchyFixed.stl";
  Model loaded(filename);
  loaded.repair();

  std::shared_ptr<Model> model = std::make_shared<Model>(filename);
  //model->repair();

  Printer  printer;
  Geometry geometry;
  

  std::string data = "";
  data += "; Farfalle GCODE Generator\n";

  printer .startup       (data);

  data += "\n; ;;;;;;;;;;;;;;;;;;";
  data += "\n; Printing:  ";
  data += "\n; ;;;;;;;;;;;;;;;;;;\n";
  
  int layerAmount = ((model->getMax()[2] - model->getMin()[2]) / geometry.layerHeight) + 1;
  std::cout << "Layer Amount: " << layerAmount << std::endl;

  for (int i = 0; i < layerAmount; i++) {
    std::cout << "Slice: " << i << std::endl;
    double h = (i + 1) * geometry.layerHeight + model->getMin()[2];
    auto lineStreaks = model->slice(glm::dvec3(0, 0, 1), h);

    for (auto streak : lineStreaks) {      
      GCode::LinearPrint line(printer);
      line.controlPoints = streak;
      line.feedrate = 0.3;
      GCode::Travel travel(printer);
      travel.controlPoints = { printer.movement->currentPosition + glm::dvec3(0,0,1) ,glm::dvec3(streak[0][0], streak[0][1],printer.movement->currentPosition[2] + 1), glm::dvec3(streak[0][0], streak[1][1],h) };
      
      printer.extruder->retract(data);
      printer.movement->travelMode(data);
      travel.toString(data);
      printer.extruder->unretract(data);
      printer.movement->printMode(data);
      line.toString(data);
      printer.extruder->retract(data);
    }
  }

  printer.shutdown(data);

  Tools::String2File("firstTest.gcode",data);
  return 0;
}