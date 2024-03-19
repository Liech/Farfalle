#include "VoxelAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"
#include "PolyglotAPI/Source/PolyglotAPI/Python/PythonEngine.h"
#include "PolyglotAPI/Source/PolyglotAPI/Lua/LuaEngine.h"

#include "Geometry/Model.h"
#include "Voxel/MarchingCubes.h"
#include "Voxel/DistanceMap.h"
#include "Voxel/CSG.h"

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

  //triangulate double volume
  std::unique_ptr<PolyglotAPI::APIFunction> triangulateDoubleAPI = std::make_unique<PolyglotAPI::APIFunction>("triangulateDouble", [this](const nlohmann::json& input) { return triangulateDouble(input); });
  triangulateDoubleAPI->setDescription(triangulateDoubleDescription());
  api.addFunction(std::move(triangulateDoubleAPI));

  //pack voxel volume
  std::unique_ptr<PolyglotAPI::APIFunction> packVolumeAPI = std::make_unique<PolyglotAPI::APIFunction>("packVolume", [this](const nlohmann::json& input) { return packVolume(input); });
  packVolumeAPI->setDescription(packVolumeDescription());
  api.addFunction(std::move(packVolumeAPI));

  //delete Double
  std::unique_ptr<PolyglotAPI::APIFunction> deleteDoubleAPI = std::make_unique<PolyglotAPI::APIFunction>("deleteDouble", [this](const nlohmann::json& input) { return deleteDouble(input); });
  deleteDoubleAPI->setDescription(packVolumeDescription());
  api.addFunction(std::move(deleteDoubleAPI));

  //distance map
  std::unique_ptr<PolyglotAPI::APIFunction> distanceMapAPI = std::make_unique<PolyglotAPI::APIFunction>("distanceMap", [this](const nlohmann::json& input) { return distanceMap(input); });
  distanceMapAPI->setDescription(distanceMapDescription());
  api.addFunction(std::move(distanceMapAPI));

  //transform distance map
  std::unique_ptr<PolyglotAPI::APIFunction> transformDistanceMapAPI = std::make_unique<PolyglotAPI::APIFunction>("transformDistanceMap", [this](const nlohmann::json& input) { return transformDistanceMap(input); });
  transformDistanceMapAPI->setDescription(transformDistanceMapDescription());
  api.addFunction(std::move(transformDistanceMapAPI));

  //union two voxel volumes
  std::unique_ptr<PolyglotAPI::APIFunction> unionVoxelAPI = std::make_unique<PolyglotAPI::APIFunction>("unionVoxel", [this](const nlohmann::json& input) { return unionVoxel(input); });
  unionVoxelAPI->setDescription(unionVoxelDescription());
  api.addFunction(std::move(unionVoxelAPI));

  //intersect two voxel volumes
  std::unique_ptr<PolyglotAPI::APIFunction> intersectVoxelAPI = std::make_unique<PolyglotAPI::APIFunction>("intersectVoxel", [this](const nlohmann::json& input) { return intersectVoxel(input); });
  intersectVoxelAPI->setDescription(intersectVoxelDescription());
  api.addFunction(std::move(intersectVoxelAPI));

  //subtract two voxel volumes
  std::unique_ptr<PolyglotAPI::APIFunction> subtractVoxelAPI = std::make_unique<PolyglotAPI::APIFunction>("subtractVoxel", [this](const nlohmann::json& input) { return subtractVoxel(input); });
  subtractVoxelAPI->setDescription(subtractVoxelDescription());
  api.addFunction(std::move(subtractVoxelAPI));

  //copy a voxel volume
  std::unique_ptr<PolyglotAPI::APIFunction> copyVoxelAPI = std::make_unique<PolyglotAPI::APIFunction>("copyVoxel", [this](const nlohmann::json& input) { return copyVoxel(input); });
  copyVoxelAPI->setDescription(copyVoxelDescription());
  api.addFunction(std::move(copyVoxelAPI));

  //copy a voxel volume
  std::unique_ptr<PolyglotAPI::APIFunction> densityAPI = std::make_unique<PolyglotAPI::APIFunction>("createDensityField", [this](const nlohmann::json& input) { return createDensityField(input); });
  densityAPI->setDescription(copyVoxelDescription());
  api.addFunction(std::move(densityAPI));

  //voxelization boundary
  std::unique_ptr<PolyglotAPI::APIFunction> voxelizationBoundaryAPI = std::make_unique<PolyglotAPI::APIFunction>("voxelizationBoundary", [this](const nlohmann::json& input) { return voxelizationBoundary(input); });
  voxelizationBoundaryAPI->setDescription(voxelizationBoundaryDescription());
  api.addFunction(std::move(voxelizationBoundaryAPI));
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
  assert(input["Resolution"[2]] % 8 == 0);
  size_t x = input["Resolution"][0];
  size_t y = input["Resolution"][1];
  size_t z = input["Resolution"][2];
  database.voxel[input["Name"]] = std::make_unique<bool[]>(x * y * z);
  database.voxelResolution[input["Name"]] = glm::ivec3(x, y, z);
  return "";
}

std::string VoxelAPI::createVolumeDescription() {
  return R"(
create a named voxel volume

createVolume({
    'Name': 'Name',
    'Resolution': [128,128,128], //dividable by 8
});
)";
}

nlohmann::json VoxelAPI::triangulateVolume(const nlohmann::json& input) {

  glm::dvec3 start = glm::dvec3(input["Start"][0], input["Start"][1], input["Start"][2]);
  glm::dvec3 end   = glm::dvec3(input["End"][0], input["End"][1], input["End"][2]);
  glm::dvec3 voxelSize = (end - start);
  glm::ivec3 resolution = glm::ivec3(input["Resolution"][0], input["Resolution"][1], input["Resolution"][2]);
  voxelSize = glm::dvec3(voxelSize.x / resolution.x, voxelSize.y / resolution.y, voxelSize.z / resolution.z);
  auto tri = MarchingCubes::polygonize(database.voxel[input["VoxelName"]].get(), start, voxelSize, resolution);
  database.models[input["ModelName"]] = std::make_unique<Model>(tri);
  return "";
}

std::string VoxelAPI::triangulateVolumeDescription() {
  return R"(
triangulate voxels

triangulateVolume({
    'VoxelName': 'VoxelBenchy',
    'ModelName': 'Benchy',
    'Resolution' : [128,128,128], //divideable by 8
    'Start' : [3,3,3],
    'End'   : [6,6,6]
});
)";
}

nlohmann::json VoxelAPI::triangulateDouble(const nlohmann::json& input) {

  glm::dvec3 start = glm::dvec3(input["Start"][0], input["Start"][1], input["Start"][2]);
  glm::dvec3 end = glm::dvec3(input["End"][0], input["End"][1], input["End"][2]);
  glm::dvec3 voxelSize = (end - start);
  glm::ivec3 resolution = glm::ivec3(input["Resolution"][0], input["Resolution"][1], input["Resolution"][2]);
  voxelSize = glm::dvec3(voxelSize.x / resolution.x, voxelSize.y / resolution.y, voxelSize.z / resolution.z);
  double isovalue = 0.5;
  if (input.contains("Isovalue"))
    isovalue = input["Isovalue"];
  auto tri = MarchingCubes::polygonize(*database.volume[input["DoubleName"]], start, voxelSize, resolution, isovalue);
  database.models[input["ModelName"]] = std::make_unique<Model>(tri);
  return "";
}

std::string VoxelAPI::triangulateDoubleDescription() {
  return R"(
triangulate double volume

triangulateDouble({
    'DoubleName': 'DoubleBenchy',
    'ModelName': 'Benchy',
    'Resolution' : [128,128,128], # divideable by 8
    'Start' : [3,3,3],
    'End'   : [6,6,6],
    'Isovalue' : 0.5  # Optional
});
)";
}

nlohmann::json VoxelAPI::packVolume(const nlohmann::json& input) {

  glm::ivec3 resolution = glm::ivec3(input["Resolution"][0], input["Resolution"][1], input["Resolution"][2]);
  glm::ivec3 packageSize = glm::ivec3(input["PackageSize"][0], input["PackageSize"][1], input["PackageSize"][2]);
  database.volume[input["DoubleName"]] = std::make_unique<std::vector<double>>(MarchingCubes::pack(database.voxel[input["VoxelName"]].get(), resolution, packageSize));
  return "";
}

std::string VoxelAPI::packVolumeDescription() {
  return R"(
pack binary voxels to double volume

triangulateVolume({
    'VoxelName': 'VoxelBenchy',
    'DoubleName': 'DoubleBenchy',
    'Resolution' : [128,128,128], //divideable by 8
    'PackageSize' : [2,2,2]
});
)";
}

nlohmann::json VoxelAPI::deleteDouble(const nlohmann::json& input) {
  database.volume.erase(input["Name"]);
  return "";
}

std::string VoxelAPI::deleteDoubleDescription() {
  return R"(
deletes a double/packed volume

deleteDouble({
    'Name': 'DoubleBenchy',
});
)";
}

nlohmann::json VoxelAPI::distanceMap(const nlohmann::json& input) {
  auto voxel = database.voxel[input["VoxelName"]].get();
  glm::ivec3 resolution = glm::ivec3(input["Resolution"][0], input["Resolution"][1], input["Resolution"][2]);
  std::string mode = input["Mode"];
  std::cout << "distanceMap::resolution: " << resolution[0] << " / " << resolution[1] << " / " << resolution[2] << std::endl;

  if (mode == "XYZ")
    database.distanceMaps[input["DistanceMapName"]] = std::make_unique<std::vector<int>>(DistanceMap<int,bool*>().distanceMap(voxel, resolution));
  else if (mode == "XY")
    database.distanceMaps[input["DistanceMapName"]] = std::make_unique<std::vector<int>>(DistanceMap<int, bool*>().distanceMapXY(voxel, resolution));
  else
    throw std::runtime_error("Meh");

  return "";
}

std::string VoxelAPI::distanceMapDescription() {
  return R"(
calculates a distance map of a voxelfield

distanceMap({
    'VoxelName'   : 'Name',
    'DistanceMapName' : 'DistanceMapName',
    'Resolution' : [128,128,128],
    'Mode' : 'XY' # XY / XYZ
});
)";
}

nlohmann::json VoxelAPI::transformDistanceMap(const nlohmann::json& input) {
  double distance = input["Distance"];
  auto& distanceMap = *database.distanceMaps[input["DistanceMapName"]];  
  auto result = DistanceMap<int>().map(distanceMap, [distance](int distanceSqrt) { return distance * distance < distanceSqrt; });
  database.voxel[input["VoxelName"]] = std::make_unique<bool[]>(result.size());
  auto destination = database.voxel[input["VoxelName"]].get();
  std::copy(result.begin(), result.end(), destination);
  return "";
}

std::string VoxelAPI::transformDistanceMapDescription() {
  return R"(
transforms a distance map back to a voxelfield

transformDistanceMap({
    'DistanceMapName' : 'DistanceMapName',
    'VoxelName'   : 'Name',
    'Distance' : 123  # in voxel
});
)";
}

nlohmann::json VoxelAPI::unionVoxel(const nlohmann::json& input) {
  auto& source = database.voxel[input["Target"]];
  auto& tool   = database.voxel[input["Tool"]];
  glm::ivec3 resolution = database.voxelResolution[input["Target"]];
  size_t dataSize = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
  CSG::orInplace(tool.get(), source.get(), dataSize);
  return "";
}

std::string VoxelAPI::unionVoxelDescription() {
  return R"(
combines a pair of voxel volumes with the OR operator. Tool is merged into Target

unionVoxel({
    'Target' : 'SourceAndResult',
    'Tool'   : 'Tool'
});
)";
}

nlohmann::json VoxelAPI::intersectVoxel(const nlohmann::json& input) {
  auto& source = database.voxel[input["Target"]];
  auto& tool = database.voxel[input["Tool"]];
  glm::ivec3 resolution = database.voxelResolution[input["Target"]];
  size_t dataSize = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
  CSG::andInplace(tool.get(), source.get(), dataSize);
  return "";
}

std::string VoxelAPI::intersectVoxelDescription() {
  return R"(
merges a pair of voxel volumes with the AND operator. Tool is merged from Target

intersectVoxel({
    'Target' : 'SourceAndResult',
    'Tool'   : 'Tool'
});
)";
}

nlohmann::json VoxelAPI::subtractVoxel(const nlohmann::json& input) {
  auto& source = database.voxel[input["Target"]];
  auto& tool = database.voxel[input["Tool"]];
  glm::ivec3 resolution = database.voxelResolution[input["Target"]];
  size_t dataSize = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
  CSG::subtractInplace(tool.get(), source.get(), dataSize);
  return "";
}

std::string VoxelAPI::subtractVoxelDescription() {
  return R"(
subtracts a pair of voxel volumes. Tool is subtracted from Target

intersectVoxel({
    'Target' : 'SourceAndResult',
    'Tool'   : 'Tool'
});
)";
}

nlohmann::json VoxelAPI::copyVoxel(const nlohmann::json& input) {
  glm::ivec3 resolution = database.voxelResolution[input["Target"]];
  bool* toCopy = database.voxel[input["Source"]].get();
  size_t dataSize = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
  database.voxel[input["Target"]] = std::make_unique<bool[]>(dataSize);
  bool* dest = database.voxel[input["Target"]].get();
  for (size_t i = 0; i < dataSize; i++)
    dest[i] = toCopy[i];

  return "";
}

std::string VoxelAPI::copyVoxelDescription() {
  return R"(
copies a voxel volume to another

copyVoxel({
    'Source' : 'Existing',
    'Target'  : 'New'
});
)";
}

nlohmann::json VoxelAPI::createDensityField(const nlohmann::json& input) {
  glm::ivec3 resolution = glm::ivec3(input["Resolution"][0], input["Resolution"][1], input["Resolution"][2]);
  size_t functionID = input["Function"];
  std::unique_ptr<std::vector<double>> result = std::make_unique<std::vector<double>>();
  result->resize(resolution.x * resolution.y * resolution.z);

  PolyglotAPI::FunctionRelay* relay;
  if (input["Language"] == "Python") {
    auto& py = PolyglotAPI::Python::PythonEngine::instance();
    relay = &PolyglotAPI::Python::PythonEngine::instance().getRelay();
  }
  else if (input["Language"] == "Lua")
    relay = &database.lua->getRelay();

  nlohmann::json inputData;
  inputData["Data"] = input["Data"];
  auto fun = [&inputData, relay, functionID](int x, int y, int z) {  
    nlohmann::json pos = nlohmann::json::array({ x, y, z });
    inputData["Pos"] = pos;
    auto result = relay->call(functionID, inputData);
    return result;
  };

  CSG::applySingleCore(*result, fun, resolution);

  database.volume[input["Name"]] = std::move(result);
  return "";
}

std::string VoxelAPI::createDensityFieldDescription() {
  return R"(
creates a double field with the given function evaluated at each position

voxelSize = 0.2;
def linearGradient(input):
  coordinate = input["Pos"];
  data = input["Data"];
  return voxelSize * (input[2] / data["Resolution"]);

createDensityField({
    'Name' : 'DoubleField', # Saved as Double field
    'Resolution': [128,128,128], # dividable by 8
    'Function' : linearGradient,
    'Language' : 'Python'  # Python / Lua . Currently Necessary to help searching the function. A little silly *shrug*
    'Data' : { "Resolution" : 128 }, # python has an issue with scope of functions that are invoked this way. As intermediate solution just give them a scope this way. Todo: Fix this issue
});
)";
}

nlohmann::json VoxelAPI::voxelizationBoundary(const nlohmann::json& input) {
  nlohmann::json result;
  auto& model = *database.models[input["Name"]];
  size_t resolution = input["Resolution"];
  size_t outerVoxels = input["OuterVoxels"];
  auto min = model.getMin();
  auto max = model.getMax();

  auto span = max - min;
  double biggest = glm::max(span.x, glm::max(span.y, span.z));

  double vlBefore = biggest / (double)resolution;
  double vlNew = vlBefore * (((double)resolution + outerVoxels) / (double)resolution);

  double offset = vlNew * (outerVoxels / 2);
  result["Min"] = nlohmann::json::array();
  result["Min"].push_back(min[0] - offset);
  result["Min"].push_back(min[1] - offset);
  result["Min"].push_back(min[2] - offset);

  result["Max"] = nlohmann::json::array();
  result["Max"].push_back(min[0] + biggest + offset);
  result["Max"].push_back(min[1] + biggest + offset);
  result["Max"].push_back(min[2] + biggest + offset);
  return result;
}

std::string VoxelAPI::voxelizationBoundaryDescription() {
  return R"(
returns an boundary for a resolution and a model that does minimizes error. Outer Voxels are the number of voxels outside of the min/max are of the model
voxelizationBoundary({
    'Name'  : 'Benchy' # Model Name
    'Resolution' : 128,  # must be uniform
    'OuterVoxels' : 2 # even number, should be higher when pack is used
});
)";
}