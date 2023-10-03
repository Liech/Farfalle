#include "FarfalleAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"

#include "Geometry/Model.h"

FarfalleAPI::FarfalleAPI() {

}

FarfalleAPI::~FarfalleAPI() {

}

void FarfalleAPI::add(PolyglotAPI::API& api, PolyglotAPI::FunctionRelay& relay) {
  //create empty voxel volume
  std::unique_ptr<PolyglotAPI::APIFunction> createVolumeAPI = std::make_unique<PolyglotAPI::APIFunction>("createVolume", createVolume);
  createVolumeAPI->setDescription(createVolumeDescription());
  api.addFunction(std::move(createVolumeAPI));

  //delete voxel volume
  std::unique_ptr<PolyglotAPI::APIFunction> deleteVolumeAPI = std::make_unique<PolyglotAPI::APIFunction>("deleteVolume", deleteVolume);
  deleteVolumeAPI->setDescription(deleteVolumeDescription());
  api.addFunction(std::move(deleteVolumeAPI));

}

nlohmann::json FarfalleAPI::deleteVolume(const nlohmann::json& input) {
  voxel.erase(input["Name"]);
  return "";
}

std::string FarfalleAPI::createVolumeDescription() {
  return R"(
deletes a voxel volume

createVolume({
    'Name': 'Name',
});
)";
}

nlohmann::json FarfalleAPI::createVolume(const nlohmann::json& input) {
  assert(input["Dimension"[2] % 8 == 0);
  size_t x = input["Dimension"][0];
  size_t y = input["Dimension"][1];
  size_t z = input["Dimension"][2];
  voxel[input["Name"]] = std::vector<bool>(x * y * z, false);
  return "";
}

std::string FarfalleAPI::createVolumeDescription() {
  return R"(
create a named voxel volume

createVolume({
    'Name': 'Name',
    'Dimension': [128,128,128], //dividable by 8
});
)";
}