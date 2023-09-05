#include "SliceMesh.h"

#include "Mesh2D.h"
#include "LineSoup.h"

SliceMesh::SliceMesh(const Model& target, std::function<double(const glm::dvec3& point)> fun, const SliceConfig& conf) :
  targetModel(target) {
  densityFunction = fun;
  config          = conf;

  sliceMesh = std::make_shared<Model>(fun, conf.CenterPoint, conf.BoundingSphereRadius*1.5, conf.Precision);
  sliceMesh->save("debug.stl");
  sliceMesh->generateUVMap();
  lineSoup = std::make_shared<LineSoup>(*sliceMesh);
}

SliceMesh::~SliceMesh() {

}

std::shared_ptr<Model> SliceMesh::getModel() {
  return sliceMesh;
}

std::vector<std::vector<glm::dvec3>> SliceMesh::getStreaks() {
  return streaks;
}

void SliceMesh::cut() {
  streaks = targetModel.slice(*sliceMesh);
  uvMesh = std::make_shared<Mesh2D>(*sliceMesh,streaks);
  sliceMesh->save("dbg/slice" + std::to_string(sliceNumber) + ".stl");
  auto triangulation = uvMesh->getTriangulation(sliceMesh.get());
  auto vertecies3d = sliceMesh->uv2World(triangulation.first);
  auto& indices = triangulation.second;
  std::unique_ptr<Model> m = std::make_unique<Model>(vertecies3d, indices);
  m->save("dbg/prep" + std::to_string(sliceNumber) + ".stl");
  resultMesh = std::move(m);
}

std::vector<std::vector<glm::dvec3>> SliceMesh::getFill(std::vector<glm::dvec3> input) {
  std::vector<std::vector<glm::dvec3>> result;

  return result;
}

std::vector<std::vector<glm::dvec3>> SliceMesh::getFill() {
  std::vector<std::vector<glm::dvec3>> result;

  for (auto x : streaks) {
    auto infill = getFill(x);
    result.insert(result.end(), infill.begin(), infill.end());
  }

  return result;
}

void SliceMesh::identifyAreas(const std::vector<std::vector<glm::dvec3>>& next) {
  projectNextToUV(next);
  cutUVs(); 

  //projectedUVMesh->savePoly("dbg/Ceiling" + std::to_string(sliceNumber) + ".svg");
  topMesh    = projectedUVMesh->difference(*uvMesh);
  insideMesh = uvMesh->difference(*projectedUVMesh);
  topMesh->savePoly("dbg/difference" + std::to_string(sliceNumber) + ".svg");

}

void SliceMesh::projectNextToUV(const std::vector<std::vector<glm::dvec3>>& next){
  std::vector<std::vector<glm::dvec3>> projected;
  for (size_t i = 0; i < next.size(); i++) {
    std::vector<glm::dvec3> sub;
    for (size_t j = 1; j < next[i].size(); j++) {
      auto& a = next[i][j - 1];
      auto& b = next[i][j];
      auto streak = sliceMesh->projectLine(a, b);
      sub.insert(sub.end(), streak.begin(), streak.end()-1);
    }

    //wrap around the loop
    auto streak = sliceMesh->projectLine(next[i].back(), next[i].back());
    sub.insert(sub.end(), streak.begin(), streak.end() - 1);

    projected.push_back(sub);
  }

  projectedUVMesh = std::make_shared<Mesh2D>(*sliceMesh, projected);
}

void SliceMesh::cutUVs() {

}

Model& SliceMesh::getResult() const {
  return *resultMesh;
}