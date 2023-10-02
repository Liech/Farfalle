#include "Macaronifyer.h"

#include <iostream>

#include "Tools/LineSimplification.h"
#include "Tools/MeshSurfaceOffsetting.h"
#include "SliceConfig.h"
#include "Model.h"

#include "Voxel/DistanceMap.h"
#include "Voxel/MarchingCubes.h"
#include "Voxel/Voxelizer.h"

/*

Marching Triangles isoline distance finding:
one full subdivision -> prohibits 3 Zero Triangles
search&mark for edges enclosing desired value
pick edge -> find if triangles have other marked edges -> travel along
  visited edge?->loop finished
  no edge left -> just streak (degenerated case)
all marked edges visted? no? ->pick one of the remaining edges and repeat
*/
Macaronifyer::Macaronifyer(Model& mainModel, const SliceConfig& config_) : config(config_){
  glm::dvec3 min = mainModel.getMin() - (mainModel.getMax() - mainModel.getMin()) / 10.0;
  glm::dvec3 max = mainModel.getMax() + (mainModel.getMax() - mainModel.getMin()) / 10.0;
  glm::dvec3 span = max - min;
  double biggest = glm::max(span.x, glm::max(span.y, span.z));
  max = min + glm::dvec3(biggest, biggest, biggest);
  span = max - min;
  glm::ivec3 resolution = glm::ivec3(1, 1, 1) * 1024;

  std::cout << "  Soup..." << std::endl;
  auto soup = mainModel.toSoup();
  std::cout << "  Voxel..." << std::endl;
  auto voxel = Voxelizer().voxelize(soup.first, soup.second, min, max, resolution);
  std::cout << "  Distance..." << std::endl;
  auto distanceMap = DistanceMap<int>().distanceMapXY(*voxel, resolution);
  voxel = nullptr;

  size_t streakCount = 0;
  double voxelSize = span.x / resolution.x;
  double distance = 0;
  while (true) {
    std::cout << "Tool: " << tools.size() << std::endl;
    std::cout << "    Erode: " << distance << " voxels" << std::endl;
    auto erosion = DistanceMap<int>().map(distanceMap, [distance](int distanceSqrt) { return distance * distance < distanceSqrt; });
    std::cout << "    pack: " << std::endl;
    auto pack = MarchingCubes::pack(erosion, resolution, glm::ivec3(2, 2, 2));
    std::cout << "    Marching Cubes" << std::endl;
    //auto tri = MarchingCubes::polygonize(erosion, min, glm::dvec3(voxelSize, voxelSize, voxelSize), resolution);
    auto tri = MarchingCubes::polygonize(pack, min, glm::dvec3(voxelSize, voxelSize, voxelSize)*2.0, resolution/2,0.5);
    std::cout << "    Triangles: " << tri.size() << std::endl;
    if (tri.size() == 0)
      break;

    auto tool = std::make_unique<Model>(tri);
    tool->save("dbg/tool" + std::to_string(streakCount) + ".stl");
    tools.push_back(std::move(tool));
    distance += config.layerWidth / voxelSize;
    streakCount++;
  }
}

Macaronifyer::~Macaronifyer() {

}

std::vector<std::vector<glm::dvec3>> Macaronifyer::macaronify(Model& toNoodlize) const {
  std::vector<std::vector<glm::dvec3>> result;
  for (int i = 0; i < tools.size(); i++) {
    std::cout << "Macaronify" << i << std::endl;
    auto noodle = toNoodlize.slice2(*tools[i]);
    std::cout << "loop " << noodle.size() << std::endl;
    result.insert(result.end(), noodle.begin(), noodle.end());
  }
  return result;
}
