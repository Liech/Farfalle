#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include "GCode/Printer.h"
#include "GCode/Movement.h"
#include "GCode/Extruder.h"
#include "GCode/Temperature.h"

class Model;
class Printer;
class FarfalleAPI;

namespace PolyglotAPI {
  namespace Lua {
    class LuaEngine;
  }
}

class apiDatabase {
public:
  apiDatabase(FarfalleAPI*);
  virtual ~apiDatabase();

  void initLua();

  std::map<std::string, std::pair<std::unique_ptr<bool[]>, glm::ivec3>>             boolField;
  std::map<std::string, std::pair<std::unique_ptr<std::vector<double>>,glm::ivec3>> doubleField;
  std::map<std::string, std::pair<std::unique_ptr<std::vector<int   >>,glm::ivec3>> intField;

  std::map<std::string, std::unique_ptr<Model>>                                    models;
  std::map<std::string, std::unique_ptr<std::vector<glm::dvec3>>>                  triangleSoup;
  std::map<std::string, std::unique_ptr<std::vector<std::vector<glm::dvec3>>>>     lines;
  std::map<std::string, nlohmann::json>                                            data;
  
  std::unique_ptr<PolyglotAPI::Lua::LuaEngine> lua;
  std::unique_ptr<Printer> printer;
  FarfalleAPI* api;
};