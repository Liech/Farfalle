#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <glm/glm.hpp>

class ModelPimpl;

class Model {
public:
  Model(const std::string& filename);
  Model(std::function<double(const glm::dvec3&)>&, const glm::dvec3& boundingSphereCenter, double boundingSphereRadius, double detail = 0.4);
  Model();

  std::vector<std::vector<glm::dvec3>> slice(const glm::dvec3& normal, double z);
  std::vector<std::vector<glm::dvec3>> slice(Model&) const;
  void repair();

  void generateUVMap();
  double getUVLayerWidth();
  glm::dvec2 getUV(const glm::dvec3& pos);

  glm::dvec3 getMin() const;
  glm::dvec3 getMax() const;
  void getBoundingSphere(glm::dvec3& center, double& radius) const;

  void save(const std::string& filename);

private:
  void init();

  std::shared_ptr<ModelPimpl> p;

  bool hasUVMap = false;
  glm::dvec3 min;
  glm::dvec3 max;
};