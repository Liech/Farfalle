#pragma once

#include <memory>
#include <array>
#include <vector>
#include <map>
#include <string>

//https://paulbourke.net/dataformats/vox/

namespace MagicaVoxImporter {
  class ChunkMAIN;

  class VoxFile {
  public:
    VoxFile(std::string filename);
    virtual ~VoxFile();

    std::vector<std::vector<unsigned char>> Models;
    std::array<std::array<unsigned char, 4>, 256>       Colors;
    std::vector<std::map<std::string, std::string>>     Materials;
  };

}