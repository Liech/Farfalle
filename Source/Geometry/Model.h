#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

class ModelPimpl;

class Model {
public:
  Model(const std::string& filename);

  std::vector<std::vector<glm::dvec3>> slice(const glm::dvec3& normal, double z);

private:
  std::shared_ptr<ModelPimpl> p;
};