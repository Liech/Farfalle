#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "apiDatabase.h"

class VoxelAPI;
class ModelAPI;
class LineAPI;
class GCodeAPI;
class ScriptAPI;

namespace PolyglotAPI {
  class API;
  class FunctionRelay;
}

class FarfalleAPI {
public:
  FarfalleAPI();
  virtual ~FarfalleAPI();

  void add(PolyglotAPI::API&, PolyglotAPI::FunctionRelay& relay);

  std::unique_ptr<PolyglotAPI::API> getAPI(PolyglotAPI::FunctionRelay&);
  apiDatabase& getDB();
private:
  std::unique_ptr<VoxelAPI>    voxel;
  std::unique_ptr<ModelAPI>    model;
  std::unique_ptr<LineAPI>     line;
  std::unique_ptr<GCodeAPI>    gcode;
  std::unique_ptr<ScriptAPI>   script;
  std::unique_ptr<apiDatabase> db;
};