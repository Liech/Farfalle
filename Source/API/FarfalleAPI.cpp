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

#include "Geometry/Model.h"

FarfalleAPI::FarfalleAPI() {

}

FarfalleAPI::~FarfalleAPI() {

}

void FarfalleAPI::add(PolyglotAPI::API& api, PolyglotAPI::FunctionRelay& relay) {
  voxel = std::make_unique<VoxelAPI>(db);
  voxel->add(api, relay);
  model = std::make_unique<ModelAPI>(db);
  model->add(api, relay);
  line  = std::make_unique<LineAPI>(db);
  line ->add(api, relay);
  gcode = std::make_unique<GCodeAPI>(db);
  gcode->add(api, relay);
}

std::unique_ptr<PolyglotAPI::API> FarfalleAPI::getAPI(PolyglotAPI::FunctionRelay& relay) {
  std::unique_ptr<PolyglotAPI::API> result = std::make_unique<PolyglotAPI::API>("Farfalle");

  add(*result, relay);

  return std::move(result);
}