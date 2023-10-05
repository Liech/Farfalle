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

  apiDatabase& database;
};