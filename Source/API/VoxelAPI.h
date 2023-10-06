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
  nlohmann::json triangulateVolume(const nlohmann::json& input);
  std::string triangulateVolumeDescription();
  nlohmann::json triangulateDouble(const nlohmann::json& input);
  std::string triangulateDoubleDescription();
  nlohmann::json packVolume(const nlohmann::json& input);
  std::string packVolumeDescription();
  nlohmann::json deleteDouble(const nlohmann::json& input);
  std::string deleteDoubleDescription();

  apiDatabase& database;
};