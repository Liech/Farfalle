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