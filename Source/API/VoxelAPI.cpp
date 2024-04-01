#include "VoxelAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"
#include "PolyglotAPI/Source/PolyglotAPI/Python/PythonEngine.h"
#include "PolyglotAPI/Source/PolyglotAPI/Lua/LuaEngine.h"

#include "Voxel/MarchingCubes.h"
#include "Voxel/DistanceMap.h"
#include "Voxel/CSG.h"
#include "Voxel/LineTracer.h"

#include "Tools/MagicaVox/VoxFile.h"


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

  //trace a voxel line
  std::unique_ptr<PolyglotAPI::APIFunction> traceVoxelLinesAPI = std::make_unique<PolyglotAPI::APIFunction>("traceVoxelLines", [this](const nlohmann::json& input) { return traceVoxelLines(input); });
  traceVoxelLinesAPI->setDescription(traceVoxelLinesDescription());
  api.addFunction(std::move(traceVoxelLinesAPI));

  //finds based on two density fields an isoline of voxels
  std::unique_ptr<PolyglotAPI::APIFunction> dualIsoVoxelAPI = std::make_unique<PolyglotAPI::APIFunction>("dualIsoVoxel", [this](const nlohmann::json& input) { return dualIsoVoxel(input); });
  dualIsoVoxelAPI->setDescription(dualIsoVoxelDescription());
  api.addFunction(std::move(dualIsoVoxelAPI));

  //loads xraw files
  std::unique_ptr<PolyglotAPI::APIFunction> loadMagicaVoxAPI = std::make_unique<PolyglotAPI::APIFunction>("loadMagicaVox", [this](const nlohmann::json& input) { return loadMagicaVox(input); });
  loadMagicaVoxAPI->setDescription(loadMagicaVoxDescription());
  api.addFunction(std::move(loadMagicaVoxAPI));

  //saves as xraw file
  std::unique_ptr<PolyglotAPI::APIFunction> saveMagicaVoxAPI = std::make_unique<PolyglotAPI::APIFunction>("saveMagicaVox", [this](const nlohmann::json& input) { return saveMagicaVox(input); });
  saveMagicaVoxAPI->setDescription(saveMagicaVoxDescription());
  api.addFunction(std::move(saveMagicaVoxAPI));

  ////voxelization boundary
  //std::unique_ptr<PolyglotAPI::APIFunction> voxelizationBoundaryAPI = std::make_unique<PolyglotAPI::APIFunction>("voxelizationBoundary", [this](const nlohmann::json& input) { return voxelizationBoundary(input); });
  //voxelizationBoundaryAPI->setDescription(voxelizationBoundaryDescription());
  //api.addFunction(std::move(voxelizationBoundaryAPI));
}

nlohmann::json VoxelAPI::deleteVolume(const nlohmann::json& input) {
  database.boolField.erase(input["Name"]);
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
  size_t x = input["Resolution"][0];
  size_t y = input["Resolution"][1];
  size_t z = input["Resolution"][2];
  if (!input.contains("Type")||input["Type"] == "Bool")
    database.boolField[input["Name"]] = std::make_pair(std::make_unique<bool[]>(x * y * z), glm::ivec3(x, y, z));
  else if (input["Type"] == "Double")
    database.doubleField[input["Name"]] = std::make_pair(std::make_unique<std::vector<double>>(x * y * z), glm::ivec3(x, y, z));
  else if (input["Type"] == "Int")
    database.intField[input["Name"]] = std::make_pair(std::make_unique<std::vector<int>>(x * y * z), glm::ivec3(x, y, z));

  return "";
}

std::string VoxelAPI::createVolumeDescription() {
  return R"(
create a named voxel volume

createVolume({
    'Name': 'Name',
    'Resolution': [128,128,128], #dividable by 8
    'Type' : 'Bool' # 'Bool' / 'Double' / 'Int'
});
)";
}

nlohmann::json VoxelAPI::triangulateVolume(const nlohmann::json& input) {

  glm::dvec3 start = glm::dvec3(input["Start"][0], input["Start"][1], input["Start"][2]);
  glm::dvec3 end   = glm::dvec3(input["End"][0], input["End"][1], input["End"][2]);
  glm::dvec3 voxelSize = (end - start);
  glm::ivec3 resolution = database.boolField[input["VoxelName"]].second;
  voxelSize = glm::dvec3(voxelSize.x / resolution.x, voxelSize.y / resolution.y, voxelSize.z / resolution.z);
  auto tri = MarchingCubes::polygonize(database.boolField[input["VoxelName"]].first.get(), start, voxelSize, resolution);

  database.triangleSoup[input["ModelName"]] = std::make_unique<std::vector<glm::dvec3>>(tri);
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

nlohmann::json VoxelAPI::triangulateDouble(const nlohmann::json& input) {

  glm::dvec3 start = glm::dvec3(input["Start"][0], input["Start"][1], input["Start"][2]);
  glm::dvec3 end = glm::dvec3(input["End"][0], input["End"][1], input["End"][2]);
  glm::dvec3 voxelSize = (end - start);
  glm::ivec3 resolution = database.doubleField[input["DoubleName"]].second;
  voxelSize = glm::dvec3(voxelSize.x / resolution.x, voxelSize.y / resolution.y, voxelSize.z / resolution.z);
  double isovalue = 0.5;
  if (input.contains("Isovalue"))
    isovalue = input["Isovalue"];
  auto tri = MarchingCubes::polygonize(*database.doubleField[input["DoubleName"]].first, start, voxelSize, resolution, isovalue);
  database.triangleSoup[input["ModelName"]] = std::make_unique<std::vector<glm::dvec3>>(tri);
  return "";
}

std::string VoxelAPI::triangulateDoubleDescription() {
  return R"(
triangulate double volume

triangulateDouble({
    'DoubleName': 'DoubleBenchy',
    'ModelName': 'Benchy',
    'Start' : [3,3,3],
    'End'   : [6,6,6],
    'Isovalue' : 0.5  # Optional
});
)";
}

nlohmann::json VoxelAPI::packVolume(const nlohmann::json& input) {

  glm::ivec3 resolution = database.boolField[input["VoxelName"]].second;
  glm::ivec3 packageSize = glm::ivec3(input["PackageSize"][0], input["PackageSize"][1], input["PackageSize"][2]);
  glm::ivec3 newResolution = glm::ivec3(resolution.x / packageSize.x, resolution.y / packageSize.y, resolution.z / packageSize.z);
  database.doubleField[input["DoubleName"]] = std::make_pair(std::make_unique<std::vector<double>>(MarchingCubes::pack(database.boolField[input["VoxelName"]].first.get(), resolution, packageSize)), newResolution);
  return "";
}

std::string VoxelAPI::packVolumeDescription() {
  return R"(
pack binary voxels to double volume

triangulateVolume({
    'VoxelName': 'VoxelBenchy',
    'DoubleName': 'DoubleBenchy',
    'PackageSize' : [2,2,2]
});
)";
}

nlohmann::json VoxelAPI::deleteDouble(const nlohmann::json& input) {
  database.doubleField.erase(input["Name"]);
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
  auto voxel = database.boolField[input["VoxelName"]].first.get();
  glm::ivec3 resolution = database.boolField[input["VoxelName"]].second;
  std::string mode = input["Mode"];
  std::cout << "distanceMap::resolution: " << resolution[0] << " / " << resolution[1] << " / " << resolution[2] << std::endl;

  if (mode == "XYZ")
    database.intField[input["DistanceMapName"]] = std::make_pair(std::make_unique<std::vector<int>>(DistanceMap<int,bool*>().distanceMap(voxel, resolution)),resolution);
  else if (mode == "XY")
    database.intField[input["DistanceMapName"]] = std::make_pair(std::make_unique<std::vector<int>>(DistanceMap<int, bool*>().distanceMapXY(voxel, resolution)),resolution);
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
    'Mode' : 'XY' # XY / XYZ
});
)";
}

nlohmann::json VoxelAPI::transformDistanceMap(const nlohmann::json& input) {
  double distance = input["Distance"];
  auto& distanceMap = *database.intField[input["DistanceMapName"]].first;  
  auto resolution = database.intField[input["DistanceMapName"]].second;
  auto result = DistanceMap<int>().map(distanceMap, [distance](int distanceSqrt) { return distance * distance < distanceSqrt; });
  database.boolField[input["VoxelName"]] = std::make_pair(std::make_unique<bool[]>(result.size()), resolution);
  auto destination = database.boolField[input["VoxelName"]].first.get();
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
  auto& source = database.boolField[input["Target"]].first;
  auto& tool   = database.boolField[input["Tool"]].first;
  glm::ivec3 resolution = database.boolField[input["Target"]].second;
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
  auto& source = database.boolField[input["Target"]].first;
  auto& tool = database.boolField[input["Tool"]].first;
  glm::ivec3 resolution = database.boolField[input["Target"]].second;
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
  auto& source = database.boolField[input["Target"]].first;
  auto& tool = database.boolField[input["Tool"]].first;
  glm::ivec3 resolution = database.boolField[input["Target"]].second;
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
  glm::ivec3 resolution = database.boolField[input["Target"]].second;
  bool* toCopy = database.boolField[input["Source"]].first.get();
  size_t dataSize = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
  database.boolField[input["Target"]] = std::make_pair(std::make_unique<bool[]>(dataSize), resolution);
  bool* dest = database.boolField[input["Target"]].first.get();
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

  database.doubleField[input["Name"]] = std::make_pair(std::move(result), resolution);
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

nlohmann::json VoxelAPI::traceVoxelLines(const nlohmann::json& input) {
  auto& source = database.boolField[input["Source"]];

  glm::dvec3 min = glm::dvec3(input["Min"][0], input["Min"][1], input["Min"][2]);
  glm::dvec3 max = glm::dvec3(input["Max"][0], input["Max"][1], input["Max"][2]);

  auto lines = LineTracer::traceLines(source.first.get(), source.second);
  std::vector<std::vector<glm::dvec3>> glmd;
  for (const auto& streak : lines) {
    std::vector<glm::dvec3> sub;
    sub.reserve(streak.size());
    for (const auto& x : streak)
      sub.push_back(x);
    glmd.push_back(sub);
  }

  auto span = max - min;
  auto resolution = source.second;
  for (auto& streak : glmd) {
    for (auto& point : streak) {
      point[0] = (point[0] / resolution[0]) * span[0] + min[0];
      point[1] = (point[1] / resolution[1]) * span[1] + min[1];
      point[2] = (point[2] / resolution[2]) * span[2] + min[2];
    }
  }
    
  if (database.lines.contains(input["Target"])) {
    auto& d = database.lines[input["Target"]];
    if (glmd.size() > 0)
      d->insert(d->begin(), glmd.begin(), glmd.end());
  }
  else
    database.lines[input["Target"]] = std::make_unique<std::vector<std::vector<glm::dvec3>>>(glmd);
  return "";
}

std::string VoxelAPI::traceVoxelLinesDescription() {
  return R"(
Traces all Lines found in a Voxel volume. 
Lines should be 1 Voxel thin, algorithm is dumb.

traceVoxelLines({
    'Source' : 'BoolField',
    'Target' : 'LineCollection',
    'Min'    : bounds["Min"],
    'Max'    : bounds["UniformMax"]
});
)";
}


nlohmann::json VoxelAPI::dualIsoVoxel(const nlohmann::json& input) {
  auto& voxelField = database.boolField[input["VoxelField"]];
  auto& resultField = database.boolField[input["ResultField"]];
  auto& densityField1 = database.doubleField[input["DensityField1"]];
  auto& densityField2 = database.doubleField[input["DensityField2"]];
  double isoValue1 = input["Isovalue1"];
  double isoValue2 = input["Isovalue2"];
  glm::ivec3 resolution = voxelField.second;
  assert(resolution == densityField1.second);
  assert(resolution == densityField2.second);
  assert(resolution == resultField.second);

  glm::ivec3 start = glm::ivec3(0, 0, 0);
  glm::ivec3 end   = resolution;

  if (input.contains("StartVoxel"))
    start = glm::ivec3(input["StartVoxel"][0], input["StartVoxel"][1], input["StartVoxel"][2]);
  if (input.contains("EndVoxel"))
    end = glm::ivec3(input["EndVoxel"][0], input["EndVoxel"][1], input["EndVoxel"][2]);

  assert(end - start == resultField.second);

  const bool* v  = voxelField.first.get();
  const auto& d1 = *densityField1.first;
  const auto& d2 = *densityField2.first;
        auto* r  = resultField.first.get();


#pragma omp parallel for
  for (long long x = start[0]; x < end.x-1; x++) {
    for (long long y = start[1]; y < end.y-1; y++) {
      for (long long z = start[2]; z < end.z-1; z++) { //memory layout main direction
        const size_t address = z + y * resolution.z + x * resolution.z * resolution.y;
        const size_t resultAddress = (z-start.z) + (y-start.y) * resultField.second.z + (x-start.x) * resultField.second.z * resultField.second.y;

        const bool d1Value = d1[address] < isoValue1;
        const bool d2Value = d2[address] < isoValue2;
        const bool vValue  = v[address];
        const bool d1X = d1[address + resolution.z * resolution.y] < isoValue1; //d1[current + glm::ivec3(1,0,0)
        const bool d1Y = d1[address + resolution.z] < isoValue1;                //d1[current + glm::ivec3(0,1,0)
        const bool d1Z = d1[address + 1] < isoValue1;                           //d1[current + glm::ivec3(0,0,1)
        const bool d2X = d2[address + resolution.z * resolution.y] < isoValue2;
        const bool d2Y = d2[address + resolution.z] < isoValue2;
        const bool d2Z = d2[address + 1] < isoValue2;
        const bool d1Diff = d1Value != d1X || d1Value != d1Y || d1Value != d1Z;
        const bool d2Diff = d2Value != d2X || d2Value != d2Y || d2Value != d2Z;

        r[resultAddress] = vValue && (d1Diff && d2Diff);
      }
    }
  }
  return "";
}

std::string VoxelAPI::dualIsoVoxelDescription() {
  return R"(
Puts the intersection line of two density field isosurfaces into a voxel field.
Expects a voxel field to be true at that point as well.

dualIsoVoxel({
    'VoxelField'     : 'BoolField',
    'ResultField'    : 'BoolField',
    'DensityField1'  : 'DoubleField',
    'DensityField2'  : 'DoubleField2',
    'Isovalue1'      : 0.3,
    'Isovalue2'      : 0.3,
    'StartVoxel'     : [0,0,0], # optional
    'EndVoxel'       : [20,20,20], # optional
});
)";
}

nlohmann::json VoxelAPI::loadMagicaVox(const nlohmann::json& input) {
  auto data = MagicaVoxImporter::VoxFile::readBinary(input["Filename"]);
  database.boolField[input["VoxelField"]] = std::make_pair(std::move(data.first), glm::ivec3(data.second[0], data.second[1], data.second[2]));
  return "";
}

std::string VoxelAPI::loadMagicaVoxDescription() {
  return R"(
Loads xraw as voxels. Can be exported with MagicaVox

loadMagicaVox({
    'VoxelField'     : 'BoolField',
    'Filename'       : 'Dataname.vox'
});
)";
}

nlohmann::json VoxelAPI::saveMagicaVox(const nlohmann::json& input) {
  auto& voxelField = database.boolField[input["VoxelField"]];
  MagicaVoxImporter::VoxFile::writeBinary(voxelField.first.get(), { (size_t)voxelField.second.x,(size_t)voxelField.second.y,(size_t)voxelField.second.z }, input["Filename"]);
  return "";
}

std::string VoxelAPI::saveMagicaVoxDescription() {
  return R"(
Saves Voxels as xraw. Can be imported with MagicaVox

saveMagicaVox({
    'VoxelField'     : 'BoolField',
    'Filename'       : 'Dataname.vox'
});
)";
}

//
//nlohmann::json VoxelAPI::voxelizationBoundary(const nlohmann::json& input) {
//  nlohmann::json result;
//  auto& model = *database.models[input["Name"]];
//  size_t resolution = input["Resolution"];
//  size_t outerVoxels = input["OuterVoxels"];
//  auto min = model.getMin();
//  auto max = model.getMax();
//
//  auto span = max - min;
//  double biggest = glm::max(span.x, glm::max(span.y, span.z));
//
//  double vlBefore = biggest / (double)resolution;
//  double vlNew = vlBefore * (((double)resolution + outerVoxels) / (double)resolution);
//
//  double offset = vlNew * (outerVoxels / 2);
//  result["Min"] = nlohmann::json::array();
//  result["Min"].push_back(min[0] - offset);
//  result["Min"].push_back(min[1] - offset);
//  result["Min"].push_back(min[2] - offset);
//
//  result["Max"] = nlohmann::json::array();
//  result["Max"].push_back(min[0] + biggest + offset);
//  result["Max"].push_back(min[1] + biggest + offset);
//  result["Max"].push_back(min[2] + biggest + offset);
//  return result;
//}
//
//std::string VoxelAPI::voxelizationBoundaryDescription() {
//  return R"(
//returns an boundary for a resolution and a model that does minimizes error. Outer Voxels are the number of voxels outside of the min/max are of the model
//voxelizationBoundary({
//    'Name'  : 'Benchy' # Model Name
//    'Resolution' : 128,  # must be uniform
//    'OuterVoxels' : 2 # even number, should be higher when pack is used
//});
//)";
//}