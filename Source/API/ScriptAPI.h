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

class ScriptAPI {
public:
  ScriptAPI(apiDatabase& db);
  virtual ~ScriptAPI();

  void add(PolyglotAPI::API&, PolyglotAPI::FunctionRelay& relay);

private:
  nlohmann::json executePythonFile(const nlohmann::json& input);
  std::string executePythonDescription();
  nlohmann::json getDocumentation(const nlohmann::json& input);
  std::string getDocumentationDescription();

  apiDatabase& database;
};