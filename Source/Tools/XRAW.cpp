#include "XRAW.h"

#include <fstream>
#include <stdexcept>

namespace XRAW {
  void XRAW::write(const std::string& filename, const bool*) {

  }

  header XRAW::readHeader(const std::string& filename) {
    std::vector<unsigned char> buffer;
    size_t position = 0;
    readBuffer(filename, buffer);
    return readHeader(buffer, position);
  }

  std::unique_ptr<bool[]> XRAW::read(const std::string& filename) {
    std::vector<unsigned char> buffer;
    size_t position = 0;
    readBuffer(filename, buffer);
    auto header = readHeader(buffer, position);
    size_t dataSize = header.getDataSize();
    std::unique_ptr<bool[]> result = std::make_unique<bool[]>(dataSize);

    size_t voxelSize = 0;
    if (header.bitsPerIndex == BitsPerIndex::NoPallete) {
      voxelSize += (size_t)header.numOfColorChannels * (size_t)header.bitsPerChannel;
    }
    else {
      voxelSize += (size_t)header.bitsPerIndex;
    }

    for (size_t i = 0; i < dataSize; i++) {
      auto data = std::vector<unsigned char>(buffer.begin() + position, buffer.begin() + position + voxelSize);
      bool content = false;
      for (size_t i = 0; i < data.size(); i++)
        content |= data[i] != 0;
      result[i] = content;
    }

    return result;
  }

  void XRAW::readBuffer(const std::string& filename, std::vector<unsigned char>& buffer) {
    std::ifstream input(filename, std::ios::binary);
    if (input.fail())
      throw std::runtime_error("Error opening " + filename);
    buffer = std::vector<unsigned char>(std::istreambuf_iterator<char>(input), {});
  }

  size_t header::getDataSize() {
    size_t dataSize = (size_t)xResolution * (size_t)yResolution * (size_t)zResolution;
    return dataSize;
  }

  header XRAW::readHeader(const std::vector<unsigned char>& buffer, size_t& position) {
    assert(std::string(buffer.begin() + position, buffer.begin() + position + 4) == "XRAW");
    position += 4;
    header result;

    result.colorChannelDataType = (ColorChannelDataType)buffer[position];
    position++;
    result.numOfColorChannels = (NumberOfColorChannels)buffer[position];
    position++;
    result.bitsPerChannel = (BitsPerChannel)buffer[position];
    position++;
    result.bitsPerIndex = (BitsPerIndex)buffer[position];
    position++;

    result.xResolution = readUInt(buffer, position);
    result.yResolution = readUInt(buffer, position);
    result.zResolution = readUInt(buffer, position);
    result.numberOfPalleteColors = (NumberOfPalleteColors)readUInt(buffer, position);
    return result;
  }

  unsigned int XRAW::readUInt(const std::vector<unsigned char>& data, size_t& position)
  {
    unsigned char bytes[] = { data[position],data[position + 1],data[position + 2],data[position + 3] };
    unsigned int* pInt = (unsigned int*)bytes;
    unsigned int result = *pInt;
    position += 4;
    return result;
  }
}