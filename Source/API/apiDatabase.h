#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>

class Model;

class apiDatabase {
public:
  std::map<std::string, std::unique_ptr<std::vector<bool>>>                    voxel;
  std::map<std::string, std::unique_ptr<std::vector<double>>>                  volume;
  std::map<std::string, std::unique_ptr<Model>>                                models;
  std::map<std::string, std::unique_ptr<std::vector<std::vector<glm::dvec3>>>> lines;
};