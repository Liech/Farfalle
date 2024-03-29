#pragma once
#include "Chunk.h"
#include <map>

namespace MagicaVoxImporter {
  class ChunkLAYR : public Chunk {
  public:
    std::string getID() override { return "LAYR"; }
    void        read(Reader own, Reader child) override;

    int LayerID;
    std::map<std::string, std::string> Attributes;
  };
}