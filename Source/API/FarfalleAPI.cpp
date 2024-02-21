#include "FarfalleAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"

#include "ModelAPI.h"
#include "VoxelAPI.h"
#include "LineAPI.h"
#include "GCodeAPI.h"
#include "ScriptAPI.h"
#include "CustomPythonFunctions.h"

#include "Geometry/Model.h"

FarfalleAPI::FarfalleAPI() {
  db     = std::make_unique<apiDatabase>(this);
  voxel  = std::make_unique<VoxelAPI>   (*db);
  model  = std::make_unique<ModelAPI>   (*db);
  line   = std::make_unique<LineAPI>    (*db);
  gcode  = std::make_unique<GCodeAPI>   (*db);
  script = std::make_unique<ScriptAPI>  (*db);  
  db->initLua();
}

FarfalleAPI::~FarfalleAPI() {

}

void FarfalleAPI::add(PolyglotAPI::API& api, PolyglotAPI::FunctionRelay& relay) {
  voxel->add(api, relay);
  model->add(api, relay);
  line ->add(api, relay);
  gcode->add(api, relay);
  script->add(api, relay);
}

std::unique_ptr<PolyglotAPI::API> FarfalleAPI::getAPI(PolyglotAPI::FunctionRelay& relay) {
  std::unique_ptr<PolyglotAPI::API> result = std::make_unique<PolyglotAPI::API>("Farfalle");

  add(*result, relay);

  return std::move(result);
}

apiDatabase& FarfalleAPI::getDB() {
  return *db;
}

void FarfalleAPI::addCustomPythonFunctions() {
  pyFun = std::make_unique<CustomPythonFunctions>();
  pyFun->add(*db);
}