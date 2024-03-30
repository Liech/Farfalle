#include "VoxFile.h"

#include <memory>

#include "ChunkMAIN.h"
#include "ChunkPACK.h"
#include "ChunkSIZE.h"
#include "ChunkXYZI.h"
#include "ChunkRGBA.h"
#include "ChunkMATL.h"

#include "VoxFileRaw.h"


namespace MagicaVoxImporter {
  VoxFile::VoxFile(const std::string& filename) {
    std::unique_ptr<Chunk> parsedFile = VoxFileRaw::read(filename);
    size_t currentChunk = 0;
    int numberOfModels = 1;
    if (parsedFile->childIsType<ChunkPACK>(currentChunk)) {
      const ChunkPACK& pack = (const ChunkPACK&)(parsedFile->getChild(currentChunk));
      numberOfModels = pack.numModels;
      currentChunk++;
    }

    for (int i = 0; i < numberOfModels; i++) {
      const ChunkSIZE& size = (const ChunkSIZE&)parsedFile->getChild(currentChunk);
      currentChunk++;
      ChunkXYZI& xyzi = (ChunkXYZI&)parsedFile->getChild(currentChunk);
      currentChunk++;
      Models.push_back({});
      auto& model = Models.back();
      size_t dataSize = (size_t)size.sizeX * (size_t)size.sizeY * (size_t)size.sizeZ;
      model.resize(dataSize);
      for (int j = 0; j < xyzi.numVoxels; j++) {
        size_t address = xyzi.content[j].X + xyzi.content[j].Y * size.sizeZ + xyzi.content[j].Z * size.sizeZ * size.sizeY;
        model[address] = xyzi.content[j].value;
      }
      Models.push_back(model);
    }


    if (!parsedFile->childIsType<ChunkRGBA>(currentChunk)) {
      auto rgba = ChunkRGBA::getDefault();
      for (int i = 0; i < 256; i++)
        Colors[i] = std::array<unsigned char, 4>{rgba->palette[i].r, rgba->palette[i].g, rgba->palette[i].b, rgba->palette[i].a};
    }
    else {
      const ChunkRGBA& rgba = (const ChunkRGBA&)parsedFile->getChild(currentChunk);
      for (int i = 0; i < 256; i++)
        Colors[i] = std::array<unsigned char, 4>{rgba.palette[i].r, rgba.palette[i].g, rgba.palette[i].b, rgba.palette[i].a};
      currentChunk++;
    }


    Materials.resize(256);
    while (currentChunk < parsedFile->numberOfChilds()) {
      const ChunkMATL& mat = (const ChunkMATL&)parsedFile->getChild(currentChunk);
      Materials[mat.MaterialID] = mat.Properties;
      currentChunk++;
    }

  }

  VoxFile::~VoxFile() {

  }
}