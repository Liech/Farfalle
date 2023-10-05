#include "VoxelAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"

#include "Geometry/Model.h"

VoxelAPI::VoxelAPI(apiDatabase& db) : database(db) {

}

VoxelAPI::~VoxelAPI() {

}

void VoxelAPI::add(PolyglotAPI::API& api, PolyglotAPI::FunctionRelay& relay) {
  //create empty voxel volume
  std::unique_ptr<PolyglotAPI::APIFunction> createVolumeAPI = std::make_unique<PolyglotAPI::APIFunction>("createVolume", [this](const nlohmann::json& input) { return createVolume(input); });
  createVolumeAPI->setDescription(createVolumeDescription());
  api.addFunction(std::move(createVolumeAPI));

  //delete voxel volume
  std::unique_ptr<PolyglotAPI::APIFunction> deleteVolumeAPI = std::make_unique<PolyglotAPI::APIFunction>("deleteVolume", [this](const nlohmann::json& input) { return deleteVolume(input); });
  deleteVolumeAPI->setDescription(deleteVolumeDescription());
  api.addFunction(std::move(deleteVolumeAPI));

  //triangulate voxel volume
  std::unique_ptr<PolyglotAPI::APIFunction> triangulateVolumeAPI = std::make_unique<PolyglotAPI::APIFunction>("triangulateVolume", [this](const nlohmann::json& input) { return triangulateVolume(input); });
  triangulateVolumeAPI->setDescription(triangulateVolumeDescription());
  api.addFunction(std::move(triangulateVolumeAPI));
}

nlohmann::json VoxelAPI::deleteVolume(const nlohmann::json& input) {
  database.voxel.erase(input["Name"]);
  return "";
}

std::string VoxelAPI::deleteVolumeDescription() {
  return R"(
deletes a voxel volume

deleteVolume({
    'Name': 'Name',
});
)";
}

nlohmann::json VoxelAPI::createVolume(const nlohmann::json& input) {
  assert(input["Dimension"[2] % 8 == 0);
  size_t x = input["Dimension"][0];
  size_t y = input["Dimension"][1];
  size_t z = input["Dimension"][2];
  database.voxel[input["Name"]] = std::make_unique<std::vector<bool>>(std::vector<bool>(x * y * z, false));
  return "";
}

std::string VoxelAPI::createVolumeDescription() {
  return R"(
create a named voxel volume

createVolume({
    'Name': 'Name',
    'Dimension': [128,128,128], //dividable by 8
});
)";
}


nlohmann::json VoxelAPI::triangulateVolume(const nlohmann::json& input) {
  
  return "";
}

std::string VoxelAPI::triangulateVolumeDescription() {
  return R"(
triangulate voxels

triangulateVolume({
    'VoxelName': 'VoxelBenchy',
    'ModelName': 'Benchy',
    'Start' : [3,3,3],
    'End'   : [6,6,6]
});
)";
}