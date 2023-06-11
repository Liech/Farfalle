#include "SliceMesh.h"

#include "Mesh2D.h"

SliceMesh::SliceMesh(const Model& target, std::function<double(const glm::dvec3& point)> fun, const SliceConfig& conf) :
  targetModel(target) {
  densityFunction = fun;
  config          = conf;

  sliceMesh = std::make_shared<Model>(fun, conf.CenterPoint, conf.BoundingSphereRadius, conf.Precision);
  sliceMesh->save("debug.stl");
  sliceMesh->generateUVMap();
}

std::shared_ptr<Model> SliceMesh::getModel() {
  return sliceMesh;
}

std::vector<std::vector<glm::dvec3>> SliceMesh::getStreaks() {
  return streaks;
}

void SliceMesh::cut() {
  streaks = targetModel.slice(*sliceMesh);
  sliceMesh->save("slice1.stl");
  uvMesh = std::make_shared<Mesh2D>(*sliceMesh,streaks);
  uvMesh->save("slice.stl");
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