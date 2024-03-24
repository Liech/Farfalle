#include "ModelAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"

#include "Voxel/Voxelizer.h"
#include "Tools/STL.h"


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

  //transform Model
  std::unique_ptr<PolyglotAPI::APIFunction> transformModelAPI = std::make_unique<PolyglotAPI::APIFunction>("transformModel", [this](const nlohmann::json& input) { return transformModel(input); });
  transformModelAPI->setDescription(transformModelDescription());
  api.addFunction(std::move(transformModelAPI));

  //transform Model
  std::unique_ptr<PolyglotAPI::APIFunction> hasAnyTriangleAPI = std::make_unique<PolyglotAPI::APIFunction>("hasAnyTriangle", [this](const nlohmann::json& input) { return hasAnyTriangle(input); });
  hasAnyTriangleAPI->setDescription(hasAnyTriangleDescription());
  api.addFunction(std::move(hasAnyTriangleAPI));

  //create plane
  std::unique_ptr<PolyglotAPI::APIFunction> createPlaneAPI = std::make_unique<PolyglotAPI::APIFunction>("createPlane", [this](const nlohmann::json& input) { return createPlane(input); });
  createPlaneAPI->setDescription(hasAnyTriangleDescription());
  api.addFunction(std::move(createPlaneAPI));
}

std::string ModelAPI::loadModelDescription() {
  return R"(
load model from file

loadModel({
    'Name' : 'Name',
    'Filename': 'Model.stl'
});
)";
}

nlohmann::json ModelAPI::loadModel(const nlohmann::json& input) {
  database.triangleSoup[input["Name"]] = STL::read(input["Filename"]);
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
  database.triangleSoup.erase(input["Name"]);
  return "";
}

std::string ModelAPI::saveModelDescription() {
  return R"(
save model to file

saveModel({
    'Name' : 'Name',
    'Filename': 'Model.stl'
});
)";
}

nlohmann::json ModelAPI::saveModel(const nlohmann::json& input) {
  STL::write(input["Filename"], *database.triangleSoup[input["Name"]],false);
  return "";
}

std::string ModelAPI::voxelizeModelDescription() {
  return R"(
voxelize model

voxelizeModel({
    'ModelName'  : 'Benchy',
    'VoxelName'  : 'VoxelizedBenchy',
    'Resolution' : [128,128,128], # divideable by 8
    'Start'      : [3,3,3],
    'End'        : [7,7,7]
});
)";
}

nlohmann::json ModelAPI::voxelizeModel(const nlohmann::json& input) {
  


  glm::dvec3 start = glm::dvec3(input["Start"][0], input["Start"][1], input["Start"][2]);
  glm::dvec3 end   = glm::dvec3(input["End"][0], input["End"][1], input["End"][2]);
  glm::ivec3 resolution = glm::ivec3(input["Resolution"][0], input["Resolution"][1], input["Resolution"][2]);

  std::unique_ptr<bool[]> voxel;
  auto soup = database.triangleSoup[input["ModelName"]].get();
  voxel = Voxelizer().voxelize(*soup, start, end, resolution);

  database.boolField[input["VoxelName"]] = std::make_pair(std::move(voxel), resolution);
  return "";
}

nlohmann::json ModelAPI::modelBoundary(const nlohmann::json& input) {
  nlohmann::json result;
  
  double padding = 0;
  if (input.contains("Padding"))
    padding = input["Padding"];

  glm::dvec3 min;
  glm::dvec3 max;
  min = glm::dvec3(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
  max = glm::dvec3(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
  const auto& tris = *database.triangleSoup[input["Name"]];
  for (size_t i = 0; i < tris.size(); i++) {
    min.x = std::min(min.x, tris[i].x);
    min.y = std::min(min.y, tris[i].y);
    min.z = std::min(min.z, tris[i].z);
    max.x = std::max(max.x, tris[i].x);
    max.y = std::max(max.y, tris[i].y);
    max.z = std::max(max.z, tris[i].z);
  }
  
  min -= glm::dvec3(padding, padding, padding);
  max += glm::dvec3(padding, padding, padding);

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

  auto span = max - min;
  double biggest = glm::max(span.x, glm::max(span.y, span.z));

  result["UniformMax"] = nlohmann::json::array();
  result["UniformMax"].push_back(min[0] + biggest);
  result["UniformMax"].push_back(min[1] + biggest);
  result["UniformMax"].push_back(min[2] + biggest);

  result["UniformSize"] = nlohmann::json::array();
  result["UniformSize"].push_back(biggest);
  result["UniformSize"].push_back(biggest);
  result["UniformSize"].push_back(biggest);

  return result;
}

std::string ModelAPI::modelBoundaryDescription() {
  return R"(
gets the boundary of a model

modelBoundary({
    'Name'  : 'Benchy',
    'Padding' : 2 # 2 greater in each direction (e.g. 2*VoxelLength)
});

result = { 
  'Min' : [3,3,3],
  'Max' : [6,6,7],
  'Size': [3,3,4],
  'UniformSize' : [4,4,4]
  'UniformMax' : [4,4,4]
};

)";
}

nlohmann::json ModelAPI::transformModel(const nlohmann::json& input) {
  auto& model = *database.triangleSoup[input["Name"]];
  glm::dvec3 offset = glm::dvec3(input["Offset"][0], input["Offset"][1], input["Offset"][2]);
  for (glm::dvec3& x : model)
    x += offset;
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

nlohmann::json ModelAPI::hasAnyTriangle(const nlohmann::json& input) {
  auto& model = *database.triangleSoup[input["Name"]];
  return model.size() != 0;
}

std::string ModelAPI::hasAnyTriangleDescription() {
  return R"(
returns True if the Model has any triangle

hasAnyTriangle({
    'Name'   : 'Name'
});
)";
}

nlohmann::json ModelAPI::createPlane(const nlohmann::json& input) {
  glm::dvec3 origin = glm::dvec3(input["Origin"][0], input["Origin"][1], input["Origin"][2]);
  glm::dvec3 normal = glm::normalize(glm::dvec3(input["Normal"][0], input["Normal"][1], input["Normal"][2]));
  double size = input["Size"];

  std::vector<glm::dvec3> result;
  result.resize(6);
  std::vector<glm::dvec3> vertices;
  vertices.resize(4);
  glm::dvec3 someRay = glm::dvec3(1, 0, 0);
  double similarity = glm::dot(someRay, normal);
  if (std::abs(similarity) > 0.9) //too samy
    someRay = glm::dvec3(0, 1, 0);

  glm::dvec3 beamA = glm::cross(normal, someRay);
  glm::dvec3 beamB = glm::cross(normal, beamA);
  vertices[0] = origin + beamA * size;
  vertices[1] = origin + beamB * size;
  vertices[2] = origin - beamA * size;
  vertices[3] = origin - beamB * size;

  result[0] = vertices[0];
  result[1] = vertices[1];
  result[2] = vertices[3];

  result[3] = vertices[1];
  result[4] = vertices[2];
  result[5] = vertices[3];

  database.triangleSoup[input["Name"]] = std::make_unique<std::vector<glm::dvec3>>(result);
  return "";
}

std::string ModelAPI::createPlaneDescription() {
  return R"(
creates a plane model consisting out of 2 triangles with the given size and orientation

createPlane({
    'Name' : 'FirstSlice',
    'Origin' : [0,0,0.2],
    'Normal' : [0,0,1],
    'Size' : 5000
});
)";
}