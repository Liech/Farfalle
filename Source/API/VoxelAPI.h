#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include "apiDatabase.h"

namespace PolyglotAPI {
  class API;
  class FunctionRelay;
}

class Model;

class VoxelAPI {
public:
  VoxelAPI(apiDatabase& db);
  virtual ~VoxelAPI();

  void add(PolyglotAPI::API&, PolyglotAPI::FunctionRelay& relay);


private:
  nlohmann::json createVolume(const nlohmann::json& input);
  std::string createVolumeDescription();
  nlohmann::json deleteVolume(const nlohmann::json& input);
  std::string deleteVolumeDescription();

  apiDatabase& database;
};