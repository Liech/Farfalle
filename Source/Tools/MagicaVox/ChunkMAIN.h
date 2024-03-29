#pragma once

#include "Chunk.h"

namespace MagicaVoxImporter {
  //main can do nothing. only having children
  class ChunkMAIN : public Chunk {
  public:
    std::string getID() override { return "MAIN"; }

  };
}