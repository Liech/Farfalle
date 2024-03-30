#include <catch2/catch_test_macros.hpp>

#include "Tools/MagicaVox/VoxFile.h"

//https://paulbourke.net/dataformats/vox/


TEST_CASE("MagicaVox/3x3") {
  auto vox = MagicaVoxImporter::VoxFile::readBinary("TestData/3x3x3.vox");
  REQUIRE(vox.second[0] == 3);
  REQUIRE(vox.second[1] == 3);
  REQUIRE(vox.second[2] == 3);
  
  auto& voxel = vox.first;

  for (size_t i = 0; i < 4;i++)
    REQUIRE(voxel[i]);
  REQUIRE(!voxel[4]);
  REQUIRE(voxel[5]);
  for (size_t i = 6; i < 10; i++)
    REQUIRE(voxel[i]);
}

TEST_CASE("MagicaVox/ReadWrite/3x3") {
  auto vox = MagicaVoxImporter::VoxFile::readBinary("TestData/3x3x3.vox");
  MagicaVoxImporter::VoxFile::writeBinary(vox.first.get(), vox.second, "3x3x3_farfalle.vox");
}