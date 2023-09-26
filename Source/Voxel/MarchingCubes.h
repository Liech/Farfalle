#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>

//https://paulbourke.net/geometry/polygonise/
class MarchingCubes {

  //static std::vector<glm::dvec3> polygonize(std::vector<double>, glm::dvec3 origin, glm::dvec3 voxelSize, double isovalue);
  static std::vector<glm::dvec3> polygonize(const std::vector<bool>&, const glm::dvec3& origin, const glm::dvec3& voxelSize);
  static std::vector<glm::dvec3> polygonize(const std::array<glm::dvec3,8>& p, const std::array<double,8>& val, double isolevel);

  //static std::vector<double> pack(std::vector<bool>)
private:
  static glm::dvec3 VertexInterp(double isolevel, const glm::dvec3& p1, const glm::dvec3& p2, double valp1, double valp2);

  static const int edgeTable[256];
  static const int triTable[256][16];
};