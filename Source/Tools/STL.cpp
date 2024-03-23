#include "STL.h"

#include <fstream>

void STL::writeUInt(std::ofstream& stream, const unsigned int& value) {
  unsigned char bytes[4];
  bytes[0] = ((value >> 24) & 0xFF);
  bytes[1] = ((value >> 16) & 0xFF);
  bytes[2] = ((value >> 8) & 0xFF);
  bytes[3] = (value & 0xFF);
  stream.write((const char*)(bytes), 4);
}

void STL::writeInt(std::ofstream& stream, const int& value) {
  unsigned char bytes[4];
  bytes[0] = ((value >> 24) & 0xFF);
  bytes[1] = ((value >> 16) & 0xFF);
  bytes[2] = ((value >> 8) & 0xFF);
  bytes[3] = (value & 0xFF);
  stream.write((const char*)(bytes), 4);
}

void STL::writeFloat(std::ofstream& stream, const float& data) {
  const float* a = &data;
  const int* b = (const int*)a;
  int    c = *b;
  writeInt(stream, data);
}

void STL::writeUShort(std::ofstream& stream, const unsigned short& value) {
  unsigned char bytes[2];
  bytes[0] = ((value >> 8) & 0xFF);
  bytes[1] = (value & 0xFF);
  stream.write((const char*)(bytes), 2);
}

unsigned int STL::readUInt(const std::vector<unsigned char>& data, size_t& position)
{
  unsigned char bytes[] = { data[position],data[position + 1],data[position + 2],data[position + 3] };
  unsigned int* pInt = (unsigned int*)bytes;
  unsigned int result = *pInt;
  position += 4;
  return result;
}

int STL::readInt(const std::vector<unsigned char>& data, size_t& position)
{
  unsigned char bytes[] = { data[position],data[position + 1],data[position + 2],data[position + 3] };
  int* pInt = (int*)bytes;
  int result = *pInt;
  position += 4;
  return result;
}

float STL::readFloat(const std::vector<unsigned char>& data, size_t& position)
{
  int  a = readInt(data, position);
  int* b = &a;
  float* c = (float*)b;
  return *c;
}

short STL::readShort(const std::vector<unsigned char>& data, size_t& position)
{
  unsigned char bytes[] = { data[position],data[position + 1] };
  short* pInt = (short*)bytes;
  short result = *pInt;
  position += 2;
  return result;
}

void STL::write(const std::string& filename, const std::vector<glm::dvec3>& tris, bool binaryFormat)
{
  if (binaryFormat)
    writeBinary(filename, tris);
  else
    writeAscii(filename, tris);
}

std::unique_ptr<std::vector<glm::dvec3>> STL::read(const std::string& filename)
{
  std::ifstream input(filename, std::ios::binary);
  if (input.fail())
    throw std::runtime_error("Error opening " + filename);
  std::vector<unsigned char> buffer = std::vector<unsigned char>(std::istreambuf_iterator<char>(input), {});
  std::string beginning = std::string(buffer.begin(), buffer.begin() + std::min(buffer.size()-1,(size_t)300));
  bool isAscii = beginning.find("facet") != std::string::npos && beginning.find("solid") != std::string::npos;
  if (isAscii)
    return readAscii(buffer);
  else
    return readBinary(buffer);
}

void STL::writeAscii(const std::string& filename, const std::vector<glm::dvec3>& data) {
  std::ofstream file;
  file.open(filename);

  std::string start = "solid https://github.com/Liech\n";
  file.write(start.c_str(),start.size());

  std::string outerLoop = "    outer loop\n";
  std::string endloop = "    endloop\n";
  std::string endfacet = "endfacet\n";

  for (size_t i = 0; i < data.size() / 3; i++) {
    glm::dvec3 a = data[i * 3 + 0];
    glm::dvec3 b = data[i * 3 + 1];
    glm::dvec3 c = data[i * 3 + 2];
    glm::dvec3 n = glm::cross(a - b, a - c);

    std::string normals = "facet normal " + std::to_string(n[0]) + " " + std::to_string(n[1]) + " " + std::to_string(n[2]) + "\n";

    std::string v1 = "        vertex " + std::to_string(a[0]) + " " + std::to_string(a[1]) + " " + std::to_string(a[2]) + "\n";
    std::string v2 = "        vertex " + std::to_string(b[0]) + " " + std::to_string(b[1]) + " " + std::to_string(b[2]) + "\n";
    std::string v3 = "        vertex " + std::to_string(c[0]) + " " + std::to_string(c[1]) + " " + std::to_string(c[2]) + "\n";

    file.write(normals.c_str(),normals.size());
    file.write(outerLoop.c_str(),outerLoop.size());
    file.write(v1.c_str(),v1.size());
    file.write(v2.c_str(),v2.size());
    file.write(v3.c_str(),v3.size());
    file.write(endloop.c_str(),endloop.size());
    file.write(endfacet.c_str(),endfacet.size());
  }

  std::string end = "endsolid https://github.com/Liech\n";
  file.write(end.c_str(),end.size());
  file.close();
}

void STL::writeBinary(const std::string& filename, const std::vector<glm::dvec3>& triangles)
{
  std::ofstream file;
  file.open(filename);
  std::string header = "FileType: Binary https://github.com/Liech                                      "; //80 byte
  file.write(header.c_str(),header.size());

  uint32_t numberOfTrianlges = triangles.size() / 3;
  writeUInt(file, numberOfTrianlges);

  for (size_t i = 0; i < numberOfTrianlges; i++)
  {
    glm::dvec3 a = triangles[i * 3 + 0];
    glm::dvec3 b = triangles[i * 3 + 1];
    glm::dvec3 c = triangles[i * 3 + 2];
    glm::dvec3 n = glm::cross(a - b, a - c);

    writeFloat (file, n[0]);
    writeFloat (file, n[1]);
    writeFloat (file, n[2]);
    writeFloat (file, a[0]);
    writeFloat (file, a[1]);
    writeFloat (file, a[2]);
    writeFloat (file, b[0]);
    writeFloat (file, b[1]);
    writeFloat (file, b[2]);
    writeFloat (file, c[0]);
    writeFloat (file, c[1]);
    writeFloat (file, c[2]);
    writeUShort(file, 0);
  }
  file.close();
}

std::unique_ptr<std::vector<glm::dvec3>>  STL::readAscii(const std::vector<unsigned char>& buffer) {
  std::unique_ptr<std::vector<glm::dvec3>> result = std::make_unique<std::vector<glm::dvec3>>();
  std::string data = std::string(buffer.begin(), buffer.end());
  std::istringstream iss(data);
  std::string line;
  std::getline(iss, line); //header is uninteresting
  for (; std::getline(iss, line); )
  {
    if (line.starts_with("vertex"))
    {
      std::istringstream lineStream(line);
      std::string element;
      glm::dvec3 subresult;
      std::getline(lineStream, element); //"vertex" -> throw away

      std::getline(lineStream, element);
      subresult.x = std::stod(element);
      std::getline(lineStream, element);
      subresult.y = std::stod(element);
      std::getline(lineStream, element);
      subresult.z = std::stod(element);
      result->push_back(subresult);      
    }
  }

  return std::move(result);
}

std::unique_ptr<std::vector<glm::dvec3>>  STL::readBinary(const std::vector<unsigned char>& buffer) {
  std::unique_ptr<std::vector<glm::dvec3>> result = std::make_unique<std::vector<glm::dvec3>>();
  size_t position = 80; //header first 80 bytes are ignored
  uint32_t numberOfTrianlges = readUInt(buffer, position);
  result->reserve(numberOfTrianlges * 3);
  
  for (size_t i = 0; i < numberOfTrianlges; i++)
  {
    glm::dvec3 n;
    n.x = readFloat(buffer, position);
    n.y = readFloat(buffer, position);
    n.z = readFloat(buffer, position);
    glm::dvec3 a;
    a.x = readFloat(buffer, position);
    a.y = readFloat(buffer, position);
    a.z = readFloat(buffer, position);
    glm::dvec3 b;
    b.x = readFloat(buffer, position);
    b.y = readFloat(buffer, position);
    b.z = readFloat(buffer, position);
    glm::dvec3 c;
    c.x = readFloat(buffer, position);
    c.y = readFloat(buffer, position);
    c.z = readFloat(buffer, position);
    readShort(buffer, position); // "Attribute byte count" Not used
    result->push_back(a);
    result->push_back(b);
    result->push_back(c);
  }

  return std::move(result);
}