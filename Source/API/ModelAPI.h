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

class ModelAPI {
public:
  ModelAPI(apiDatabase& db);
  virtual ~ModelAPI();

  void add(PolyglotAPI::API&, PolyglotAPI::FunctionRelay& relay);

private:
  nlohmann::json loadModel(const nlohmann::json& input);
  std::string loadModelDescription();
  nlohmann::json deleteModel(const nlohmann::json& input);
  std::string deleteModelDescription();
  nlohmann::json saveModel(const nlohmann::json& input);
  std::string saveModelDescription();
  nlohmann::json voxelizeModel(const nlohmann::json& input);
  std::string voxelizeModelDescription();
  nlohmann::json modelBoundary(const nlohmann::json& input);
  std::string modelBoundaryDescription();
  nlohmann::json transformModel(const nlohmann::json& input);
  std::string transformModelDescription();
  nlohmann::json hasAnyTriangle(const nlohmann::json& input);
  std::string hasAnyTriangleDescription();
  nlohmann::json createPlane(const nlohmann::json& input);
  std::string createPlaneDescription();

  apiDatabase& database;
};