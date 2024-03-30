#pragma once

#include <memory>
#include <array>
#include <vector>
#include <map>
#include <string>

//https://paulbourke.net/dataformats/vox/
//https://github.com/jpaver/opengametools
//https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
//https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox-extension.txt

namespace MagicaVoxImporter {
  class ChunkMAIN;

  class VoxFile {
  public:
    VoxFile(const std::string& filename);
    virtual ~VoxFile();

    std::vector<std::vector<unsigned char>>         Models;
    std::array<std::array<unsigned char, 4>, 256>   Colors;
    std::vector<std::map<std::string, std::string>> Materials;
  };

}