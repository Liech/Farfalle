#include <catch2/catch_test_macros.hpp>

#include "Voxel/LineTracer.h"

#include <glm/gtx/hash.hpp>
#include <unordered_set>

TEST_CASE("LineTracer/Empty") {

  glm::ivec3 resolution = glm::ivec3(8, 8, 8);
  size_t dataSize = resolution.x * resolution.y * resolution.z;
  auto volume = std::make_unique<bool[]>(dataSize);
  for (size_t i = 0; i < dataSize; i++)
    volume[i] = false;
  auto result = LineTracer::traceLines(volume.get(), resolution);
  REQUIRE(result.size() == 0);
}

TEST_CASE("LineTracer/gatherTrues") {

  glm::ivec3 resolution = glm::ivec3(8, 8, 8);
  size_t dataSize = resolution.x * resolution.y * resolution.z;
  auto volume = std::make_unique<bool[]>(dataSize);
  for (size_t i = 0; i < dataSize; i++)
    volume[i] = false;

  std::vector<size_t> testPositions;
  std::vector<glm::ivec3> testCoords;

  testPositions.push_back(8);
  testPositions.push_back(19);
  testPositions.push_back(23);
  testPositions.push_back(511);
  testPositions.push_back(488);
  testPositions.push_back(0);
  
  for (size_t i = 0; i < testPositions.size(); i++) {
    size_t v = testPositions[i];
    volume[v] = true;
    testCoords.push_back(glm::ivec3(v / (resolution.z * resolution.y), (v / resolution.z)%resolution.y, v % resolution.z));
  }

  auto result = LineTracer::gatherTrues(volume.get(), resolution);
  std::unordered_set<glm::ivec3> resultSet = std::unordered_set<glm::ivec3>(result.begin(), result.end());

  REQUIRE(result.size() == testPositions.size());
  for (size_t i = 0; i < testPositions.size(); i++) {
    REQUIRE(resultSet.contains(testCoords[i]));
  }
}