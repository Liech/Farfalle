#pragma once

#include <memory>
#include <vector>
#include <array>
#include <string>
#include <functional>
#include <glm/glm.hpp>

class ModelPimpl;
class Mesh2D;

class Model {
public:
  Model(const std::string& filename);
  Model(std::function<double(const glm::dvec3&)>&, const glm::dvec3& boundingSphereCenter, double boundingSphereRadius, double detail = 0.4);
  Model(const std::vector<glm::dvec3>& vertecies, const std::vector<int>& indices);
  Model();

  std::vector<std::vector<glm::dvec3>> slice(const glm::dvec3& normal, double z);
  std::vector<std::vector<glm::dvec3>> slice(Model&) const;
  std::vector<std::vector<glm::dvec3>> slice2(Model&) const;
  std::vector<std::vector<glm::dvec3>> getBorder() const;
  std::vector<glm::dvec3> projectLine(const glm::dvec3& start, const glm::dvec3& end);
  void repair();

  void generateUVMap();
  double getUVLayerWidth();
  glm::dvec2 world2UV(const glm::dvec3& pos) const;
  glm::dvec3 uv2World(const glm::dvec2& uv)  const;
  std::vector<glm::dvec3> uv2World(const std::vector<glm::dvec2>& uv)  const;

  glm::dvec3 getMin() const;
  glm::dvec3 getMax() const;
  void getBoundingSphere(glm::dvec3& center, double& radius) const;

  void save(const std::string& filename) const;
  std::unique_ptr<Model> from2D(const Mesh2D&) const;

  size_t                getNumberFaces() const;
  std::array<size_t, 3> getFaceIndices(size_t number) const;
  glm::dvec3            getVertexPosition(size_t idx) const;

private:
  void init();

  std::shared_ptr<ModelPimpl> p;

  bool hasUVMap = false;
  glm::dvec3 min;
  glm::dvec3 max;
};