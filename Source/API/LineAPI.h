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

class LineAPI {
public:
  LineAPI(apiDatabase& db);
  virtual ~LineAPI();

  void add(PolyglotAPI::API&, PolyglotAPI::FunctionRelay& relay);

private:
  nlohmann::json deleteLines(const nlohmann::json& input);
  std::string deleteLinesDescription();

  apiDatabase& database;
};