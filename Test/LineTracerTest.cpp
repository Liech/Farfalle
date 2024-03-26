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


TEST_CASE("LineTracer/Two/Z") {
  glm::ivec3 resolution = glm::ivec3(8, 8, 8);
  size_t dataSize = resolution.x * resolution.y * resolution.z;
  auto volume = std::make_unique<bool[]>(dataSize);
  for (size_t i = 0; i < dataSize; i++)
    volume[i] = false;

  glm::ivec3 a = glm::ivec3(4, 4, 3);
  glm::ivec3 b = glm::ivec3(4, 4, 4);
  size_t aAddress = a.z + a.y * resolution.z + a.x * resolution.x * resolution.y;
  size_t bAddress = b.z + b.y * resolution.z + b.x * resolution.x * resolution.y;
  volume[aAddress] = true;
  volume[bAddress] = true;

  auto result = LineTracer::traceLines(volume.get(), resolution);
  REQUIRE(result.size() == 1);
  REQUIRE(result[0].size() == 2);

  std::unordered_set<glm::ivec3> resultSet = std::unordered_set<glm::ivec3>(result[0].begin(), result[0].end());
  REQUIRE(resultSet.contains(a));
  REQUIRE(resultSet.contains(b));
}


TEST_CASE("LineTracer/Two/Y") {
  glm::ivec3 resolution = glm::ivec3(8, 8, 8);
  size_t dataSize = resolution.x * resolution.y * resolution.z;
  auto volume = std::make_unique<bool[]>(dataSize);
  for (size_t i = 0; i < dataSize; i++)
    volume[i] = false;

  glm::ivec3 a = glm::ivec3(4, 3, 4);
  glm::ivec3 b = glm::ivec3(4, 4, 4);
  size_t aAddress = a.z + a.y * resolution.z + a.x * resolution.x * resolution.y;
  size_t bAddress = b.z + b.y * resolution.z + b.x * resolution.x * resolution.y;
  volume[aAddress] = true;
  volume[bAddress] = true;

  auto result = LineTracer::traceLines(volume.get(), resolution);
  REQUIRE(result.size() == 1);
  REQUIRE(result[0].size() == 2);

  std::unordered_set<glm::ivec3> resultSet = std::unordered_set<glm::ivec3>(result[0].begin(), result[0].end());
  REQUIRE(resultSet.contains(a));
  REQUIRE(resultSet.contains(b));
}

TEST_CASE("LineTracer/Two/X") {
  glm::ivec3 resolution = glm::ivec3(8, 8, 8);
  size_t dataSize = resolution.x * resolution.y * resolution.z;
  auto volume = std::make_unique<bool[]>(dataSize);
  for (size_t i = 0; i < dataSize; i++)
    volume[i] = false;

  glm::ivec3 a = glm::ivec3(3, 4, 4);
  glm::ivec3 b = glm::ivec3(4, 4, 4);
  size_t aAddress = a.z + a.y * resolution.z + a.x * resolution.x * resolution.y;
  size_t bAddress = b.z + b.y * resolution.z + b.x * resolution.x * resolution.y;
  volume[aAddress] = true;
  volume[bAddress] = true;

  auto result = LineTracer::traceLines(volume.get(), resolution);
  REQUIRE(result.size() == 1);
  REQUIRE(result[0].size() == 2);

  std::unordered_set<glm::ivec3> resultSet = std::unordered_set<glm::ivec3>(result[0].begin(), result[0].end());
  REQUIRE(resultSet.contains(a));
  REQUIRE(resultSet.contains(b));
}

TEST_CASE("LineTracer/Straigth/Z") {
  glm::ivec3 resolution = glm::ivec3(8, 8, 8);
  size_t dataSize = resolution.x * resolution.y * resolution.z;
  auto volume = std::make_unique<bool[]>(dataSize);
  for (size_t i = 0; i < dataSize; i++)
    volume[i] = false;

  glm::ivec3 a = glm::ivec3(4, 4, 2);
  glm::ivec3 b = glm::ivec3(4, 4, 6);

  for (size_t i = a.z; i <= b.z; i++) {
    size_t address = i + a.y * resolution.z + a.x * resolution.x * resolution.y;
    volume[address] = true;
  }

  auto result = LineTracer::traceLines(volume.get(), resolution);
  REQUIRE(result.size() == 1);
  REQUIRE(result[0].size() == 2);

  std::unordered_set<glm::ivec3> resultSet = std::unordered_set<glm::ivec3>(result[0].begin(), result[0].end());
  REQUIRE(resultSet.contains(a));
  REQUIRE(resultSet.contains(b));
}

TEST_CASE("LineTracer/Straigth/X") {
  glm::ivec3 resolution = glm::ivec3(8, 8, 8);
  size_t dataSize = resolution.x * resolution.y * resolution.z;
  auto volume = std::make_unique<bool[]>(dataSize);
  for (size_t i = 0; i < dataSize; i++)
    volume[i] = false;

  glm::ivec3 a = glm::ivec3(0 ,4, 4);
  glm::ivec3 b = glm::ivec3(7 ,4, 4);

  for (size_t i = a.x; i <= b.x; i++) {
    size_t address = a.z + a.y * resolution.z + i * resolution.x * resolution.y;
    volume[address] = true;
  }

  auto result = LineTracer::traceLines(volume.get(), resolution);
  REQUIRE(result.size() == 1);
  REQUIRE(result[0].size() == 2);

  std::unordered_set<glm::ivec3> resultSet = std::unordered_set<glm::ivec3>(result[0].begin(), result[0].end());
  REQUIRE(resultSet.contains(a));
  REQUIRE(resultSet.contains(b));
}

TEST_CASE("LineTracer/Straigth/Y") {
  glm::ivec3 resolution = glm::ivec3(8, 8, 8);
  size_t dataSize = resolution.x * resolution.y * resolution.z;
  auto volume = std::make_unique<bool[]>(dataSize);
  for (size_t i = 0; i < dataSize; i++)
    volume[i] = false;

  glm::ivec3 a = glm::ivec3(4, 0, 4);
  glm::ivec3 b = glm::ivec3(4, 7, 4);

  for (size_t i = a.y; i <= b.y; i++) {
    size_t address = a.x + i * resolution.z + a.x * resolution.x * resolution.y;
    volume[address] = true;
  }

  auto result = LineTracer::traceLines(volume.get(), resolution);
  REQUIRE(result.size() == 1);
  REQUIRE(result[0].size() == 2);

  std::unordered_set<glm::ivec3> resultSet = std::unordered_set<glm::ivec3>(result[0].begin(), result[0].end());
  REQUIRE(resultSet.contains(a));
  REQUIRE(resultSet.contains(b));
}