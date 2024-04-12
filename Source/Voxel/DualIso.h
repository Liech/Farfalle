#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>


/// <summary>
/// Creates the intersection of two isosurfaces in voxelspace
/// similar to two marching cubes and a triangle intersection test between both, but with voxels
/// </summary>
class DualIso
{
public:
  template<typename T> static void dualIso(const bool* input, const std::vector<T>& d1, const std::vector<T>& d2, double isoValue1, double isoValue2, const glm::ivec3& resolution,const glm::ivec3& start, const glm::ivec3& end, bool* result, const glm::ivec3& resultResolution){
    size_t dataSize = (size_t)resolution.x + (size_t)resolution.y + (size_t)resolution.z;
    const size_t xOff = resolution.z * resolution.y;
    const size_t yOff = resolution.z;
    const size_t zOff = 1;

#pragma omp parallel for
    for (long long x = start[0]; x < end.x - 1; x++) {
      for (long long y = start[1]; y < end.y - 1; y++) {
        for (long long z = start[2]; z < end.z - 1; z++) { //memory layout main direction
          const size_t address = z + y * resolution.z + x * resolution.z * resolution.y;
          const size_t resultAddress = (z - start.z) + (y - start.y) * resultResolution.z + (x - start.x) * resultResolution.z * resultResolution.y;

          const bool d1Value = d1[address] < isoValue1;
          const bool d2Value = d2[address] < isoValue2;
          const bool vValue = input[address];

          const bool d1X = d1[address + xOff] < isoValue1;
          const bool d1Y = d1[address + yOff] < isoValue1;
          const bool d1Z = d1[address + zOff] < isoValue1;
          const bool d1Xm = (address > xOff) ? d1[address - xOff] < isoValue1 : d1Value;
          const bool d1Ym = (address > yOff) ? d1[address - yOff] < isoValue1 : d1Value;
          const bool d1Zm = (address > zOff) ? d1[address - zOff] < isoValue1 : d1Value;

          const bool d2X = d2[address + xOff] < isoValue2;
          const bool d2Y = d2[address + yOff] < isoValue2;
          const bool d2Z = d2[address + zOff] < isoValue2;
          const bool d2Xm = (address > xOff) ? d2[address - xOff] < isoValue2 : d2Value;
          const bool d2Ym = (address > yOff) ? d2[address - yOff] < isoValue2 : d2Value;
          const bool d2Zm = (address > zOff) ? d2[address - zOff] < isoValue2 : d2Value;


          const bool d1Diff = d1Value != d1X || d1Value != d1Y || d1Value != d1Z || d1Value != d1Xm || d1Value != d1Ym || d1Value != d1Zm;
          const bool d2Diff = d2Value != d2X || d2Value != d2Y || d2Value != d2Z || d2Value != d2Xm || d2Value != d2Ym || d2Value != d2Zm;

          result[resultAddress] = vValue && (d1Diff && d2Diff);
        }
      }
    }
  }
};