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

class GCodeAPI {
public:
  GCodeAPI(apiDatabase& db);
  virtual ~GCodeAPI();

  void add(PolyglotAPI::API&, PolyglotAPI::FunctionRelay& relay);

private:
  nlohmann::json saveText(const nlohmann::json& input);
  std::string saveTextDescription();
  nlohmann::json linearPrint(const nlohmann::json& input);
  std::string linearPrintDescription();
  nlohmann::json setHeat(const nlohmann::json& input);
  std::string setHeatDescription();
  nlohmann::json shutdown(const nlohmann::json& input);
  std::string shutdownDescription();
  nlohmann::json startup(const nlohmann::json& input);
  std::string startupDescription();
  nlohmann::json prime(const nlohmann::json& input);
  std::string primeDescription();

  apiDatabase& database;
};