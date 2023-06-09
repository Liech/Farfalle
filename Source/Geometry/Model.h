#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

class ModelPimpl;

class Model {
public:
  Model(const std::string& filename);
  Model();

  std::vector<std::vector<glm::dvec3>> slice(const glm::dvec3& normal, double z);
  std::shared_ptr<Model> erode(double radius) const;
  void repair();

  glm::dvec3 getMin() const;
  glm::dvec3 getMax() const;

private:
  void init();

  std::shared_ptr<ModelPimpl> p;

  glm::dvec3 min;
  glm::dvec3 max;
};