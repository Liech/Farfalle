#include <catch2/catch_test_macros.hpp>

#include "Tools/MagicaVox/VoxFile.h"

//https://paulbourke.net/dataformats/vox/


TEST_CASE("MagicaVox/Empty") {
  auto vox = MagicaVoxImporter::VoxFile::VoxFile("TestData/3x3x3.vox");
  
  REQUIRE(vox.Models.size() == 2);
  REQUIRE(vox.Models[0][10] == 0);
}