#include "ModelAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"

#include "Voxel/Voxelizer.h"

#include "Geometry/Model.h"

ModelAPI::ModelAPI(apiDatabase& db) : database(db) {

}

ModelAPI::~ModelAPI() {

}

void ModelAPI::add(PolyglotAPI::API& api, PolyglotAPI::FunctionRelay& relay) {
  //load model from file
  std::unique_ptr<PolyglotAPI::APIFunction> loadModelAPI = std::make_unique<PolyglotAPI::APIFunction>("loadModel", [this](const nlohmann::json& input) { return loadModel(input); });
  loadModelAPI->setDescription(loadModelDescription());
  api.addFunction(std::move(loadModelAPI));

  //repair model
  std::unique_ptr<PolyglotAPI::APIFunction> repairModelAPI = std::make_unique<PolyglotAPI::APIFunction>("repairModel", [this](const nlohmann::json& input) { return repairModel(input); });
  repairModelAPI->setDescription(repairModelDescription());
  api.addFunction(std::move(repairModelAPI));

  //save model
  std::unique_ptr<PolyglotAPI::APIFunction> saveModelAPI = std::make_unique<PolyglotAPI::APIFunction>("saveModel", [this](const nlohmann::json& input) { return saveModel(input); });
  saveModelAPI->setDescription(saveModelDescription());
  api.addFunction(std::move(saveModelAPI));

  //voxelize model
  std::unique_ptr<PolyglotAPI::APIFunction> voxelizeModelAPI = std::make_unique<PolyglotAPI::APIFunction>("voxelizeModel", [this](const nlohmann::json& input) { return voxelizeModel(input); });
  voxelizeModelAPI->setDescription(voxelizeModelDescription());
  api.addFunction(std::move(voxelizeModelAPI));
}

std::string ModelAPI::loadModelDescription() {
  return R"(
load model from file

loadModel({
    'Name' : 'Name',
    'Filename': 'fil/name/of/model.stl'
});
)";
}

nlohmann::json ModelAPI::loadModel(const nlohmann::json& input) {
  database.models[input["Name"]] = std::make_unique<Model>(input["Filename"]);
  return "";
}

std::string ModelAPI::repairModelDescription() {
  return R"(
repairs model

repairModel({
    'Name' : 'Name'
});
)";
}

nlohmann::json ModelAPI::repairModel(const nlohmann::json& input) {
  database.models[input["Name"]]->repair();
  return "";
}

std::string ModelAPI::saveModelDescription() {
  return R"(
save model to file

saveModel({
    'Name' : 'Name',
    'Filename': 'fil/name/of/model.stl'
});
)";
}

nlohmann::json ModelAPI::saveModel(const nlohmann::json& input) {
  database.models[input["Name"]]->save(input["Filename"]);
  return "";
}

std::string ModelAPI::voxelizeModelDescription() {
  return R"(
voxelize model

voxelizeModel({
    'ModelName'  : 'Benchy',
    'VoxelName'  : 'VoxelizedBenchy',
    'Resolution' : [128,128,128], //divideable by 8
    'Start'      : [3,3,3],
    'End'        : [7,7,7]
});
)";
}

nlohmann::json ModelAPI::voxelizeModel(const nlohmann::json& input) {
  auto soup = database.models[input["ModelName"]]->toSoup();
  glm::dvec3 start = glm::dvec3(input["Start"][0], input["Start"][1], input["Start"][2]);
  glm::dvec3 end   = glm::dvec3(input["End"][0], input["End"][1], input["End"][2]);
  glm::ivec3 resolution = glm::ivec3(input["Resolution"][0], input["Resolution"][1], input["Resolution"][2]);
  auto voxel = Voxelizer().voxelize(soup.first, soup.second, start, end, resolution);
  database.voxel[input["VoxelName"]] = std::move(voxel);
  return "";
}