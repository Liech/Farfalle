#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

class Voxelizer
{
public:
  std::unique_ptr<std::vector<bool>> voxelize(const std::vector<glm::dvec3>& vertecies, const std::vector<size_t>& indices, const glm::dvec3& start, const glm::dvec3& end, const glm::ivec3& resolution);

  void boxelize(const std::vector<bool>& data, const glm::dvec3& start, const glm::dvec3& end, const glm::ivec3& resolution, std::vector<glm::dvec3>& out_triangles);

private:
  void box(const glm::dvec3& a, const glm::dvec3& b, const glm::dvec3& c, glm::dvec2& start, glm::dvec2& end);
  bool intersectRayTriangle(const glm::dvec3& rayStart, const glm::dvec3& rayDir, const glm::dvec3& A, const glm::dvec3& B, const glm::dvec3& C, glm::dvec3& out);
};