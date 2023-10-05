#include "FarfalleAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"

#include "ModelAPI.h"
#include "VoxelAPI.h"

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
}
