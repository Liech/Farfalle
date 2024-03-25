#include "LineTracer.h"

#include <unordered_set>

std::vector<std::vector<glm::ivec3>> LineTracer::traceLines(const bool* data, const glm::ivec3& resolution) {
  std::vector<std::vector<glm::ivec3>> result;

  auto trueBits = gatherTrues(data, resolution);
  //std::unordered_set<glm::ivec3> trueVoxels = std::unordered_set<glm::ivec3>(trueBits.begin(), trueBits.end());
  if (trueBits.size() != 0)
    result.push_back(trueBits);
  return result;
}

std::vector<glm::ivec3> LineTracer::gatherTrues(const bool* data, const glm::ivec3& resolution) {
  std::vector<glm::ivec3> result;  
  size_t threadAmount = 8;
  size_t dataSize = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
  assert(dataSize % threadAmount == 0);

  std::vector<std::vector<glm::ivec3>> subResults;
  subResults.resize(threadAmount);

  size_t chunk = dataSize / 8;
#pragma omp parallel for
  for (long long thread = 0; thread < threadAmount; thread++) {
    size_t start = chunk * thread;
    for (size_t i = 0; i < chunk; i++) {
      size_t pos = i + start;
      if (data[pos]) {
        size_t x = pos / (resolution.y * resolution.z);
        size_t y = (pos % (resolution.y * resolution.z)) / resolution.z;
        size_t z = pos % resolution.z;
        glm::ivec3 coord = glm::ivec3(x, y, z);
        subResults[thread].push_back(coord);
      }
    }
  }
  for (size_t i = 0; i < threadAmount; i++) {
    result.insert(result.end(), subResults[i].begin(), subResults[i].end());
  }

  return result;
}
