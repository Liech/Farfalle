#include "ModelAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"

#include "Voxel/Voxelizer.h"
#include "Geometry/Mesh2D.h"

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

  //delete model
  std::unique_ptr<PolyglotAPI::APIFunction> deleteModelAPI = std::make_unique<PolyglotAPI::APIFunction>("deleteModel", [this](const nlohmann::json& input) { return deleteModel(input); });
  deleteModelAPI->setDescription(deleteModelDescription());
  api.addFunction(std::move(deleteModelAPI));

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

  //model boundary
  std::unique_ptr<PolyglotAPI::APIFunction> modelBoundaryAPI = std::make_unique<PolyglotAPI::APIFunction>("modelBoundary", [this](const nlohmann::json& input) { return modelBoundary(input); });
  modelBoundaryAPI->setDescription(modelBoundaryDescription());
  api.addFunction(std::move(modelBoundaryAPI));

  //slice Model
  std::unique_ptr<PolyglotAPI::APIFunction> sliceModelAPI = std::make_unique<PolyglotAPI::APIFunction>("sliceModel", [this](const nlohmann::json& input) { return sliceModel(input); });
  sliceModelAPI->setDescription(modelBoundaryDescription());
  api.addFunction(std::move(sliceModelAPI));

  //transform Model
  std::unique_ptr<PolyglotAPI::APIFunction> transformModelAPI = std::make_unique<PolyglotAPI::APIFunction>("transformModel", [this](const nlohmann::json& input) { return transformModel(input); });
  transformModelAPI->setDescription(transformModelDescription());
  api.addFunction(std::move(transformModelAPI));
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

std::string ModelAPI::deleteModelDescription() {
  return R"(
delete model from memory

deleteModel({
    'Name' : 'Name'
});
)";
}

nlohmann::json ModelAPI::deleteModel(const nlohmann::json& input) {
  database.models.erase(input["Name"]);
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

nlohmann::json ModelAPI::modelBoundary(const nlohmann::json& input) {
  nlohmann::json result;
  auto& model = *database.models[input["Name"]];
  auto min = model.getMin();
  auto max = model.getMax();
  result["Min"] = nlohmann::json::array();
  result["Min"].push_back(min[0]);
  result["Min"].push_back(min[1]);
  result["Min"].push_back(min[2]);

  result["Max"] = nlohmann::json::array();
  result["Max"].push_back(max[0]);
  result["Max"].push_back(max[1]);
  result["Max"].push_back(max[2]);

  result["Size"] = nlohmann::json::array();
  result["Size"].push_back(max[0] - min[0]);
  result["Size"].push_back(max[1] - min[1]);
  result["Size"].push_back(max[2] - min[2]);

  return result;
}

std::string ModelAPI::modelBoundaryDescription() {
  return R"(
gets the boundary of a model

modelBoundary({
    'Name'  : 'Benchy'
});

result = { 
  'Min' : [3,3,3],
  'Max' : [6,6,6],
  'Size': [3,3,3]
};

)";
}

nlohmann::json ModelAPI::sliceModel(const nlohmann::json& input) {
  auto& model = *database.models[input["ModelName"]];
  auto& tool  = *database.models[input["ToolName"]];
  std::string mode = input["Mode"];
  if (mode == "Model") {
    auto streaks = model.slice(tool);
    auto uvMesh = Mesh2D(tool, streaks);
    auto triangulation = uvMesh.getTriangulation(&tool);
    auto vertecies3d = tool.uv2World(triangulation.first);
    auto& indices = triangulation.second;
    std::unique_ptr<Model> m = std::make_unique<Model>(vertecies3d, indices);
    database.models[input["ResultName"]] = std::move(m);
  }
  else if (mode == "Line") {
    auto noodle = model.slice2(tool);
    database.lines[input["LineName"]] = std::make_unique<std::vector<std::vector<glm::dvec3>>>(noodle);
  }
  else {
    throw std::runtime_error("Unkown Mode");
  }

  return "";
}

std::string ModelAPI::sliceModelDescription() {
  return R"(
slice model with model

sliceModel({
    'ModelName'  : 'Benchy',
    'ToolName'   : 'ModelPlane',
    'Mode'       : 'Line',
    'LineName'   : 'HalfBakedGCode'
});

sliceModel({
    'ModelName'  : 'Benchy',
    'ToolName'   : 'ModelPlane',
    'Mode'       : 'Model',
    'ResultName' : 'BenchySliceModel'
});
)";
}

nlohmann::json ModelAPI::transformModel(const nlohmann::json& input) {
  auto& model = *database.models[input["Name"]];
  auto soup = model.toSoup();
  glm::dvec3 offset = glm::dvec3(input["Offset"][0], input["Offset"][1], input["Offset"][2]);
  for (glm::dvec3& x : soup.first)
    x += offset;
  database.models[input["Name"]] = std::make_unique<Model>(soup.first, soup.second);
  return "";
}

std::string ModelAPI::transformModelDescription() {
  return R"(
moves model around

transformModel({
    'Name'   : 'Name',
    'Offset' : [1,1,1]
});
)";
}