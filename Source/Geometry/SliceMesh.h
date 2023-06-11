#pragma once

#include <memory>
#include <functional>

#include <glm/glm.hpp>

#include "SliceConfig.h"
#include "Model.h"

class Model;
class Mesh2D;

class SliceMesh {
public:
  SliceMesh(const Model& target, std::function<double(const glm::dvec3& point)>, const SliceConfig& config);

  void cut();

  std::shared_ptr<Model> getModel();
  std::vector<std::vector<glm::dvec3>> getStreaks();
  std::vector<std::vector<glm::dvec3>> getFill();

private:
  std::vector<std::vector<glm::dvec3>> getFill(std::vector<glm::dvec3>);

  SliceConfig             config;
  const Model&            targetModel;
  std::shared_ptr<Model>  sliceMesh;
  std::shared_ptr<Mesh2D> uvMesh;
  std::vector<std::vector<glm::dvec3>> streaks;
  std::function<double(const glm::dvec3& point)> densityFunction;
};