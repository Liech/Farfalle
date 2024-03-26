#include "LineTracer.h"

#include <glm/gtx/hash.hpp>
#include <glm/gtx/closest_point.hpp>
#include <unordered_set>

std::vector<std::vector<glm::ivec3>> LineTracer::traceLines(const bool* data, const glm::ivec3& resolution) {
  std::vector<std::vector<glm::ivec3>> result;

  std::unordered_map<glm::ivec3, std::pair<bool, size_t>> lineEndMap;

  std::vector<glm::ivec3> todo = gatherTrues(data, resolution);
  std::unordered_set<glm::ivec3> trueVoxels = std::unordered_set<glm::ivec3>(todo.begin(), todo.end());
  std::unordered_set<glm::ivec3> visited;

  std::vector<glm::ivec3> neighbourPos = getNeighbours();
  size_t neighbourAmount               = neighbourPos.size();

  while (todo.size() > 0) {
    glm::ivec3 current = todo.back();
    todo.pop_back();
    visited.insert(current);

    for (size_t i = 0; i < neighbourAmount; i++) {
      glm::ivec3 nPos = neighbourPos[i] + current;
      if (!visited.contains(nPos) && trueVoxels.contains(nPos)) {
        todo.push_back(nPos);
        
        auto search = lineEndMap.find(current);
        if (search != lineEndMap.end()) {
          bool& atBeginning = search->second.first;
          auto& foundList = result[search->second.second];
          if (atBeginning)
            foundList.insert(foundList.begin(), nPos);
          else
            foundList.push_back(nPos);
          lineEndMap[nPos] = std::make_pair(atBeginning, search->second.second);
        }
        else {
          std::vector<glm::ivec3> sub;
          sub.push_back(current);
          sub.push_back(nPos);
          result.push_back(sub);
          lineEndMap[current] = std::make_pair(false, result.size() - 1);
          lineEndMap[nPos   ] = std::make_pair(true, result.size() - 1);
        }

      }
    }
  }
  
  for (auto& streak : result) {
    removePointsOnLine(streak);
  }

  return result;
}


void LineTracer::removePointsOnLine(std::vector<glm::ivec3>& line) {

}

constexpr std::vector<glm::ivec3> LineTracer::getNeighbours() {
  std::vector<glm::ivec3> neighbours;
  for (int x = -1; x <= 1; x++)
    for (int y = -1; y <= 1; y++)
      for (int z = -1; z <= 1; z++)
        if (!(x == 0 && y == 0 && z == 0))
          neighbours.push_back(glm::ivec3(x, y, z));
  return neighbours;
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
