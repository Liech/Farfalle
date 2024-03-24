#include <catch2/catch_test_macros.hpp>

#include "Voxel/LineTracer.h"

TEST_CASE("LineTracer/Base") {

  glm::ivec3 resolution = glm::ivec3(3, 3, 3);
  size_t dataSize = resolution.x * resolution.y * resolution.z;
  auto volume = std::make_unique<bool[]>(dataSize);
  for (size_t i = 0; i < dataSize; i++)
    volume[i] = false;
  auto result = LineTracer::traceLines(volume.get(), resolution);
  REQUIRE(result.size() == 0);
}