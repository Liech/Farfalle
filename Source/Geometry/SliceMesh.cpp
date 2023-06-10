#include "SliceMesh.h"


SliceMesh::SliceMesh(const Model& target, std::function<double(const glm::dvec3& point)> fun, const SliceConfig& conf) :
  targetModel(target) {
  densityFunction = fun;
  config          = conf;

  sliceMesh = std::make_shared<Model>(fun, conf.CenterPoint, conf.BoundingSphereRadius, conf.Precision);
  sliceMesh->generateUVMap("h:UV");
  sliceMesh->save("debug.stl");
}

std::shared_ptr<Model> SliceMesh::getModel() {
  return sliceMesh;
}

std::vector<std::vector<glm::dvec3>> SliceMesh::getStreaks() {
  return streaks;
}

void SliceMesh::cut() {
  streaks = targetModel.slice(*sliceMesh);
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