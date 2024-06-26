#pragma once

#include "Chunk.h"

//https://github.com/ephtracy/voxel-model/issues/19

namespace MagicaVoxImporter {
  //Render Objects Chunk
  class ChunkrOBJ : public  Chunk {
  public:
    std::string getID() const override { return "rOBJ"; }
    void        read(Reader own, Reader child) override;
  };
}