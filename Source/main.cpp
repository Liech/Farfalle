#include <iostream>
#include <functional>

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


std::shared_ptr<Model> genSliceTool(const Model& input, double z) {

  glm::dvec3 center;
  double radius;
  double precision = 4;
  input.getBoundingSphere(center, radius);
  center.z = input.getMin()[2];
  std::cout << "  Gen Slice Tool..." << std::endl;  

  std::function<double(const glm::dvec3& point)> sphereFun = [center, z,radius, precision](const glm::dvec3& point) {
    if (point.z < center.z) {
      if (point.z > center.z - radius/2.0) {
        if ((glm::distance(glm::dvec3(point.x, point.y, 0), glm::dvec3(center.x, center.y, 0)) - z) < 0)
          return -1.0;
        else
          return 1.0;
      }
      else
        return -1.0;
    }

    return glm::distance(point, center) - z;
  };

  double offset = input.getMin()[2];
  std::function<double(const glm::dvec3& point) > planeFun = [center, z, offset, precision](const glm::dvec3& point) {
    return point.z - (z+ offset);
  };

  std::shared_ptr<Model> result = std::make_shared<Model>(sphereFun, center, radius*2, precision);
  result->generateUVMap("h:UV");
  result->save("debug.stl");
  return result;
}

int main() {
  
  std::string filename = "C:\\Users\\nicol\\Downloads\\_3DBenchy_-_The_jolly_3D_printing_torture-test_by_CreativeTools_se_763622\\files\\3DBenchyFixed.stl";

  std::shared_ptr<Model> model = std::make_shared<Model>(filename);
  model->repair();
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
    std::cout << "Slice: " << i << "/" << layerAmount << std::endl;
    double h = (i + 1) * geometry.layerHeight;

    //auto lineStreaks = model->slice(glm::dvec3(0, 0, 1), h); //Planar slicing
    auto tool = genSliceTool(*model, h);
    std::cout << "  Slice..." << std::endl;
    auto lineStreaks = model->slice(*tool); //Implicit Surface slicing 

    std::cout << "  Gather Streaks..." << std::endl;
    for (auto streak : lineStreaks) {      
      GCode::LinearPrint line(printer);
      line.controlPoints = streak;
      line.feedrate = 0.03;
      GCode::Travel travel(printer);
      travel.controlPoints = { printer.movement->currentPosition + glm::dvec3(0,0,1) ,glm::dvec3(streak[0][0], streak[0][1],printer.movement->currentPosition[2] + 1), streak[0] };
      
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

  Tools::String2File("output.gcode",data);
  return 0;
}