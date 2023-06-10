#pragma once

#include <memory>
#include <functional>

#include <glm/glm.hpp>

#include "SliceConfig.h"
#include "Model.h"

class Model;

class SliceMesh {
public:
  SliceMesh(const Model& target, std::function<double(const glm::dvec3& point)>, const SliceConfig& config);

  std::shared_ptr<Model> getModel();

private:
  SliceConfig            config;
  const Model&           targetModel;
  std::shared_ptr<Model> sliceMesh;
  std::function<double(const glm::dvec3& point)> densityFunction;
};