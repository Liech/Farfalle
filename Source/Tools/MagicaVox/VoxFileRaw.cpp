#include "VoxFileRaw.h"

#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>

#include "Chunk.h"
#include "Reader.h"

namespace MagicaVoxImporter {
  VoxFileRaw::VoxFileRaw() {

  }

  VoxFileRaw::~VoxFileRaw() {

  }

  std::unique_ptr<Chunk> VoxFileRaw::read(const std::string& filename) {
    std::ifstream input(filename, std::ios::binary);
    if (input.fail())
      throw std::runtime_error("Error opening " + filename);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    Reader content(buffer);
    std::string magicByte = content.readChunkID();
    if (magicByte != "VOX ")
      throw std::runtime_error("Magic Byte not found! No a MagicaVox File!");
    int version = content.readInt();
    if (version != 150 && version != 200)
      throw std::runtime_error("Unkown MagicaVox File Version. Supported: 150 && 200");
    std::unique_ptr<Chunk> chunk = content.readChunk();
    //chunk->print();
    return std::move(chunk);
  }
}