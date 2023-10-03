#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace PolyglotAPI {
  class API;
  class FunctionRelay;
}

class Model;

class FarfalleAPI {
public:
  FarfalleAPI();
  virtual ~FarfalleAPI();

  void add(PolyglotAPI::API&, PolyglotAPI::FunctionRelay& relay);


private:
  nlohmann::json createVolume(const nlohmann::json& input);
  std::string createVolumeDescription();
  nlohmann::json deleteVolume(const nlohmann::json& input);
  std::string deleteVolumeDescription();

  std::map<std::string, std::vector<bool>>                   voxel;
  std::map<std::string, std::vector<std::unique_ptr<Model>>> models;
};