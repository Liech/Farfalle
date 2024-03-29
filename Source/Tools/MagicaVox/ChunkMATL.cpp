#include "ChunkMATL.h"

#include "Reader.h"
#include <map>

namespace MagicaVoxImporter {

  void ChunkMATL::read(Reader own, Reader child) {
    Chunk::read(own, child);

    MaterialID = own.readInt();
    Properties = own.readDict();
  }
}