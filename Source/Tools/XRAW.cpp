#include "XRAW.h"

#include <fstream>
#include <stdexcept>

namespace XRAW {
  void XRAW::write(const std::string& filename, const bool* data, size_t xResolution, size_t yResolution, size_t zResolution) {
    header h;
    h.colorChannelDataType = ColorChannelDataType::UnsignedInteger;
    h.numOfColorChannels = NumberOfColorChannels::R;
    h.bitsPerChannel = BitsPerChannel::Short;
    h.bitsPerIndex = BitsPerIndex::NoPallete;
    h.xResolution = xResolution;
    h.xResolution = yResolution;
    h.xResolution = zResolution;
    h.numberOfPalleteColors = NumberOfPalleteColors::Amount256;

    std::ofstream file;
    file.open(filename);
    writeHeader(file,h);

    size_t dataSize = h.getDataSize();

    for (size_t i = 0; i < dataSize; i++) {
      writeUShort(file, data[i] ? 1 : 0);
    }
    file.close();
  }

  header XRAW::readHeader(const std::string& filename) {
    std::vector<unsigned char> buffer;
    size_t position = 0;
    readBuffer(filename, buffer);
    return readHeader(buffer, position);
  }

  std::unique_ptr<bool[]> XRAW::readBool(const std::string& filename) {
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

  void XRAW::writeHeader(std::ofstream& stream, const header& input) {
    stream.write("XRAW", 4);
    unsigned char colorChannelDataType = (char)input.colorChannelDataType;
    stream.write((const char*)colorChannelDataType, 1);
    unsigned char numOfColorChannels = (char)input.numOfColorChannels;
    stream.write((const char*)numOfColorChannels, 1);
    unsigned char bitsPerChannel = (char)input.bitsPerChannel;
    stream.write((const char*)bitsPerChannel, 1);
    unsigned char bitsPerIndex = (char)input.bitsPerIndex;
    stream.write((const char*)bitsPerIndex, 1);
    writeUInt(stream, input.xResolution);
    writeUInt(stream, input.yResolution);
    writeUInt(stream, input.zResolution);
    writeUInt(stream, (unsigned int)input.numberOfPalleteColors);
  }

  unsigned int XRAW::readUInt(const std::vector<unsigned char>& data, size_t& position)
  {
    unsigned char bytes[] = { data[position],data[position + 1],data[position + 2],data[position + 3] };
    unsigned int* pInt = (unsigned int*)bytes;
    unsigned int result = *pInt;
    position += 4;
    return result;
  }

  void XRAW::writeUInt(std::ofstream& stream, const unsigned int& value) {
    unsigned char bytes[4];
    bytes[0] = ((value >> 24) & 0xFF);
    bytes[1] = ((value >> 16) & 0xFF);
    bytes[2] = ((value >> 8) & 0xFF);
    bytes[3] = (value & 0xFF);
    stream.write((const char*)(bytes), 4);
  }
  void XRAW::writeUShort(std::ofstream& stream, const unsigned short& value) {
    unsigned char bytes[2];
    bytes[0] = ((value >> 8) & 0xFF);
    bytes[1] = (value & 0xFF);
    stream.write((const char*)(bytes), 2);
  }
}