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
  nlohmann::json distanceMap(const nlohmann::json& input);
  std::string distanceMapDescription();
  nlohmann::json transformDistanceMap(const nlohmann::json& input);
  std::string transformDistanceMapDescription();
  nlohmann::json unionVoxel(const nlohmann::json& input);
  std::string unionVoxelDescription();
  nlohmann::json intersectVoxel(const nlohmann::json& input);
  std::string intersectVoxelDescription();
  nlohmann::json subtractVoxel(const nlohmann::json& input);
  std::string subtractVoxelDescription();
  nlohmann::json copyVoxel(const nlohmann::json& input);
  std::string copyVoxelDescription();
  nlohmann::json createDensityField(const nlohmann::json& input);
  std::string createDensityFieldDescription();
  nlohmann::json traceVoxelLines(const nlohmann::json& input);
  std::string traceVoxelLinesDescription();
  nlohmann::json dualIsoVoxel(const nlohmann::json& input);
  std::string dualIsoVoxelDescription();
  nlohmann::json loadMagicaVox(const nlohmann::json& input);
  std::string loadMagicaVoxDescription();
  nlohmann::json saveMagicaVox(const nlohmann::json& input);
  std::string saveMagicaVoxDescription();
  nlohmann::json int2double(const nlohmann::json& input);
  std::string int2doubleDescription();

  //nlohmann::json voxelizationBoundary(const nlohmann::json& input);
  //std::string voxelizationBoundaryDescription();

  apiDatabase& database;
};