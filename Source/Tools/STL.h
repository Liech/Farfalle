#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <sstream>

class STL {
public:
  static void write(const std::string& filename, const std::vector<glm::dvec3>&, bool binaryFormat = false);
  static std::vector<glm::dvec3> read(const std::string& filename);

private:
  static void writeAscii(const std::string& filename, const std::vector<glm::dvec3>&);
  static void writeBinary(const std::string& filename, const std::vector<glm::dvec3>&);
  static std::vector<glm::dvec3> readAscii(const std::vector<unsigned char>&);
  static std::vector<glm::dvec3> readBinary(const std::vector<unsigned char>&);

  static void writeUInt(std::ofstream& stream, const unsigned int& value);
  static void writeInt(std::ofstream& stream, const int& value);
  static void writeFloat(std::ofstream& stream, const float& data);
  static void writeUShort(std::ofstream& stream, const unsigned short& value);

  static unsigned int readUInt (const std::vector<unsigned char>& data, size_t& position);
  static int          readInt  (const std::vector<unsigned char>& data, size_t& position);
  static float        readFloat(const std::vector<unsigned char>& data, size_t& position);
  static short        readShort(const std::vector<unsigned char>& data, size_t& position)
};