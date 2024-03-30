#include "ChunkSIZE.h"

#include <string>
#include <iostream>

#include "Tools/MagicaVox/IO/Reader.h"

namespace MagicaVoxImporter {
  void ChunkSIZE::read(Reader own, Reader child) {
    Chunk::read(own, child);
    sizeX = own.readInt();
    sizeY = own.readInt();
    sizeZ = own.readInt();
  }

  void ChunkSIZE::print(int indentation) {
    Chunk::print(indentation);
    std::cout << std::string(indentation + 1, ' ') << sizeX << std::endl;
    std::cout << std::string(indentation + 1, ' ') << sizeY << std::endl;
    std::cout << std::string(indentation + 1, ' ') << sizeZ << std::endl;
  }
}