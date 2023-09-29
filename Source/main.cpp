#include <iostream>
#include <functional>

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include "Tools/String2File.h"
#include "Tools/STLWriter.h"

#include "GCode/Travel.h"
#include "GCode/LinearPrint.h"
#include "GCode/Printer.h"
#include "GCode/Temperature.h"
#include "GCode/Extruder.h"
#include "GCode/Movement.h"

#include "Geometry.h"
#include "Geometry/Model.h"
#include "Geometry/SliceMesh.h"
#include "Geometry/Mesh2D.h"
#include "Geometry/Spaghettifyer.h"
#include "Geometry/Macaronifyer.h"
#include "Geometry/Volume.h"
#include "Geometry/Primitives.h"

#include "Voxel/Voxelizer.h"
#include "Voxel/MarchingCubes.h"
#include "Voxel/DistanceMap.h"

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

void nef(Model& model) {

  std::cout << "Create sphere..." << std::endl;
  auto sphere = Primitives::sphere(glm::dvec3(0, 0, 0), 1,4,4);
  std::cout << "Save sphere..." << std::endl;
  sphere->save("dbg/minkowskiSphere.stl");
  std::cout << "Volumize sphere..." << std::endl;
  auto volSphere = sphere->getVolume();
  std::cout << "Back sphere..." << std::endl;
  auto backSphere = volSphere->getModel();
  backSphere->save("dbg/backAndForthSphere.stl");
  std::cout << "Start conversion..." << std::endl;
  auto vol = model.getVolume();
  std::cout << "Minkowski..." << std::endl;
  vol->minkowski(*volSphere);
  std::cout << "Start back conversion..." << std::endl;
  auto back = vol->getModel();
  std::cout << "Saving..." << std::endl;
  back->save("dbg/backAndForth.stl");
  std::cout << "Saved" << std::endl;
}

void slice() {
  Printer  printer;
  Geometry geometry;

  bool spaghetti = false;
  bool macaroni = true;

  //std::string filename = "C:\\Users\\Niki\\Downloads\\_3DBenchy_-_The_jolly_3D_printing_torture-test_by_CreativeTools_se_763622\\files\\3DBenchyFixed.stl";
  std::string filename = "C:\\Users\\nicol\\Downloads\\_3DBenchy_-_The_jolly_3D_printing_torture-test_by_CreativeTools_se_763622\\files\\3DBenchyFixed.stl";
  //std::string filename = "C:\\Users\\nicol\\OneDrive\\3dDruck\\Modelle\\Gifts\\Flexi-Rex-improved.stl";

  std::cout << "Loading Mesh: " << filename << std::endl;
  std::shared_ptr<Model> model = std::make_shared<Model>(filename);
  //model->repair();

  //nef(*model);

  int layerAmount = ((model->getMax()[2] - model->getMin()[2]) / geometry.layerHeight) + 1;
  //layerAmount = 20;
  int startLayer = 0;

  SliceConfig config;
  model->getBoundingSphere(config.CenterPoint, config.BoundingSphereRadius);
  config.BoundingSphereRadius *= 1.3;
  config.Precision = 70;
  config.layerWidth = geometry.layerWidth;

  std::vector<std::shared_ptr<SliceMesh>> tools;

  for (int i = 0; i < layerAmount; i++) {
    std::cout << "Generate Tools: " << i << "/" << layerAmount << std::endl;
    double h = (i + startLayer + 1) * geometry.layerHeight;
    tools.push_back(genSliceTool(*model, h, config));
    tools.back()->sliceNumber = i;
  }

  int progress = 0;
//#pragma omp parallel for
//  for (int i = 0; i < layerAmount; i++) {
//    tools[i]->cut();
//
//#pragma omp atomic
//    progress++;
//    std::cout << "Slice: " << progress << "/" << layerAmount << std::endl;
//  }

  std::vector<std::vector<glm::dvec3>> pasta;
  if (spaghetti) {
    Spaghettifyer noodlizer(*model, config);
    for (int i = 0; i < layerAmount; i++) {
      std::cout << "Spaghettification: " << i << "/" << layerAmount << std::endl;
      auto subPasta = noodlizer.spaghettify(tools[i]->getResult());
      pasta.insert(pasta.end(), subPasta.begin(), subPasta.end());
    }
  }
  if (macaroni) {
    Macaronifyer noodlizer(*model, config);
    for (int i = 0; i < layerAmount; i++) {
      std::cout << "Macaronification: " << i << "/" << layerAmount << std::endl;
      auto subPasta = noodlizer.macaronify(*tools[i]->getModel());
      pasta.insert(pasta.end(), subPasta.begin(), subPasta.end());
    }
  }

  //  progress = 0;
  //#pragma omp parallel for
  //  for (int i = 0; i < layerAmount-1; i++) {
  //    tools[i]->identifyAreas(tools[i + 1]->getStreaks());
  //
  //#pragma omp atomic
  //    progress++;
  //    std::cout << "Projection: " << progress << "/" << layerAmount << std::endl;
  //  }

  std::string data = "";
  data += "; Farfalle GCODE Generator\n";

  printer.startup(data);

  data += "\n; ;;;;;;;;;;;;;;;;;;";
  data += "\n; Printing:  ";
  data += "\n; ;;;;;;;;;;;;;;;;;;\n";


  for (int i = 0; i < pasta.size(); i++) {
    std::cout << "GCode: " << i << "/" << pasta.size() << std::endl;
    auto& streak = pasta[i];
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

  printer.shutdown(data);

  Tools::String2File("dbg/output.gcode", data);
}

int main() {
  slice();
  //Voxelizer v;

  //std::vector<glm::dvec3> triangles;
  //std::vector<glm::dvec3> triangles2;

  //std::string filename = "C:\\Users\\nicol\\Downloads\\_3DBenchy_-_The_jolly_3D_printing_torture-test_by_CreativeTools_se_763622\\files\\3DBenchyFixed.stl";
  //std::shared_ptr<Model> model = std::make_shared<Model>(filename);
  //auto boat = model->toSoup();
  //glm::ivec3 resolution = glm::ivec3(1, 1, 1)*128;
  //std::cout << "voxelize..." << std::endl;
  //glm::dvec3 min = model->getMin() - (model->getMax()- model->getMin())/10.0;
  //glm::dvec3 max = model->getMax() + (model->getMax() - model->getMin()) / 10.0;
  //glm::dvec3 span = max-min;
  //glm::dvec3 voxelSize = glm::dvec3(span.x / resolution.x, span.y / resolution.y, span.z / resolution.z);

  //auto vox = v.voxelize(boat.first, boat.second, min, max, resolution);
  ////std::cout << "boxelize..." << std::endl;
  ////v.boxelize(*vox, model->getMin(), model->getMax(), resolution, triangles2);
  ////std::cout << "pack..." << std::endl;
  ////int packFactor = 8;
  ////auto pack = MarchingCubes::pack(*vox, resolution, glm::ivec3(1, 1, 1) * packFactor);
  //std::cout << "marching cubes..." << std::endl;
  //auto distanceMap = DistanceMap<int>().distanceMap(*vox, resolution);

  //for (int i = 0; i < 30; i++) {
  //  std::cout << i << std::endl;
  //  auto unpack = DistanceMap<int>().map(distanceMap, [i](int distance) { return distance > i; });
  //  triangles2 = MarchingCubes::polygonize(unpack, min, voxelSize, resolution);
  //  STLWriter::write("dbg/erode" + std::to_string(i) + ".stl", triangles2);
  //}

  //v.boxelize({ true}, glm::dvec3(0.3, 0.3, 0.3), glm::dvec3(0.6, 0.6, 0.6), glm::ivec3(1,1,1), triangles);

  //std::vector<size_t> indices;
  //for (size_t i = 0; i < triangles.size(); i++)
  //  indices.push_back(i);

  //glm::ivec3 resolution = glm::ivec3(16, 16, 16);
  //glm::dvec3 start = glm::dvec3(-1, 0, 0);
  //glm::dvec3 end = glm::dvec3(2,1,1);
  //auto vox = v.voxelize(triangles, indices, start, end, resolution);
  //v.boxelize(*vox, start, end, resolution, triangles2);

  //STLWriter::write("dbg/box.stl", triangles2);
}