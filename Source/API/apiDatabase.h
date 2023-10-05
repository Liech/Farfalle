#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

class Model;

class apiDatabase {
public:
  std::map<std::string, std::vector<bool>>                   voxel;
  std::map<std::string, std::vector<std::unique_ptr<Model>>> models;
};