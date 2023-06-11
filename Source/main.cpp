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
#include "Geometry/SliceMesh.h"
#include "Geometry/Mesh2D.h"

std::shared_ptr<SliceMesh> genSliceTool(const Model& input, double z, const SliceConfig& config) {
  std::function<double(const glm::dvec3& point)> sphereFun = [z, &config](const glm::dvec3& point) {
    if (point.z < config.CenterPoint.z) {
      if (point.z > config.CenterPoint.z - config.BoundingSphereRadius/2.0) {
        if ((glm::distance(glm::dvec3(point.x, point.y, 0), glm::dvec3(config.CenterPoint.x, config.CenterPoint.y, 0)) - z) < 0)
          return -1.0;
        else
          return 1.0;
      }
      else
        return -1.0;
    }

    return glm::distance(point, config.CenterPoint) - z;
  };

  double offset = input.getMin()[2];
  std::function<double(const glm::dvec3& point) > planeFun = [z, offset](const glm::dvec3& point) {
    return point.z - (z+ offset);
  };

  return std::make_shared<SliceMesh>(input, planeFun, config);
}

int main() {
  Printer  printer;
  Geometry geometry;
  
  std::string filename = "C:\\Users\\nicol\\Downloads\\_3DBenchy_-_The_jolly_3D_printing_torture-test_by_CreativeTools_se_763622\\files\\3DBenchyFixed.stl";
  //std::string filename = "C:\\Users\\nicol\\OneDrive\\3dDruck\\Modelle\\Gifts\\Flexi-Rex-improved.stl";

  std::cout << "Loading Mesh: " << filename << std::endl;
  std::shared_ptr<Model> model = std::make_shared<Model>(filename);
  //model->repair();

  int layerAmount = ((model->getMax()[2] - model->getMin()[2]) / geometry.layerHeight) + 1;
  layerAmount = 1;
  SliceConfig config;
  model->getBoundingSphere(config.CenterPoint, config.BoundingSphereRadius);
  config.BoundingSphereRadius *= 1.3;
  config.Precision = 20;
  config.layerWidth = geometry.layerWidth;

  std::vector<std::shared_ptr<SliceMesh>> tools;

  for (int i = 0; i < layerAmount; i++) {
    std::cout << "Generate Tools: " << i << "/" << layerAmount << std::endl;
    double h = (i + 1) * geometry.layerHeight;
    tools.push_back(genSliceTool(*model, h, config));
  }

  std::vector<std::vector<std::vector<glm::dvec3>>> streaks;
  streaks.resize(layerAmount);
  int progress = 0;
  //#pragma omp parallel for
  for (int i = 0; i < layerAmount; i++) {
    //#pragma omp atomic
    progress++;

    std::cout << "Slice: " << progress << "/" << layerAmount << std::endl;
    //double h = (i + 1) * geometry.layerHeight;
    //streaks.push_back(model->slice(glm::dvec3(0, 0, 1), h); //Planar slicing
    tools[i]->cut();
  }

  std::string data = "";
  data += "; Farfalle GCODE Generator\n";

  printer .startup       (data);

  data += "\n; ;;;;;;;;;;;;;;;;;;";
  data += "\n; Printing:  ";
  data += "\n; ;;;;;;;;;;;;;;;;;;\n";
  

  for (int i = 0; i < layerAmount; i++) {
    std::cout << "GCode: " << i << "/" << layerAmount << std::endl;

    for (auto streak : tools[i]->getStreaks()) {
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