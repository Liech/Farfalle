#include "ScriptAPI.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"
#include "PolyglotAPI/Source/PolyglotAPI/Python/PythonEngine.h"
#include "PolyglotAPI/Source/PolyglotAPI/Lua/LuaEngine.h"
#include "API/FarfalleAPI.h"

ScriptAPI::ScriptAPI(apiDatabase& db) : database(db) {

}

ScriptAPI::~ScriptAPI() {

}

void ScriptAPI::add(PolyglotAPI::API& api, PolyglotAPI::FunctionRelay& relay) {
  //executes script
  std::unique_ptr<PolyglotAPI::APIFunction> pythonCallAPI = std::make_unique<PolyglotAPI::APIFunction>("executePythonFile", [this](const nlohmann::json& input) { return executePythonFile(input); });
  pythonCallAPI->setDescription(executePythonDescription());
  api.addFunction(std::move(pythonCallAPI));
  //executes script
  std::unique_ptr<PolyglotAPI::APIFunction> luaCallAPI = std::make_unique<PolyglotAPI::APIFunction>("executeLuaFile", [this](const nlohmann::json& input) { return executeLuaFile(input); });
  luaCallAPI->setDescription(executePythonDescription());
  api.addFunction(std::move(luaCallAPI));
  //returns api documentation
  std::unique_ptr<PolyglotAPI::APIFunction> documentationAPI = std::make_unique<PolyglotAPI::APIFunction>("getDocumentation", [this](const nlohmann::json& input) { return getDocumentation(input); });
  documentationAPI->setDescription(getDocumentationDescription());
  api.addFunction(std::move(documentationAPI));
  //gets custom data
  std::unique_ptr<PolyglotAPI::APIFunction> getDataAPI = std::make_unique<PolyglotAPI::APIFunction>("getData", [this](const nlohmann::json& input) { return getData(input); });
  getDataAPI->setDescription(getDataDescription());
  api.addFunction(std::move(getDataAPI));
  //resets custom data
  std::unique_ptr<PolyglotAPI::APIFunction> setDataAPI = std::make_unique<PolyglotAPI::APIFunction>("setData", [this](const nlohmann::json& input) { return setData(input); });
  setDataAPI->setDescription(setDataDescription());
  api.addFunction(std::move(setDataAPI));
  //sets current directory
  std::unique_ptr<PolyglotAPI::APIFunction> setDirectoryAPI = std::make_unique<PolyglotAPI::APIFunction>("setDirectory", [this](const nlohmann::json& input) { return setDirectory(input); });
  setDirectoryAPI->setDescription(setDirectoryDescription());
  api.addFunction(std::move(setDirectoryAPI));
}

std::string ScriptAPI::executePythonDescription() {
  return R"(
calls a python file

executePythonFile({
    'Filename' : 'main.py'
});
)";
}

nlohmann::json ScriptAPI::executePythonFile(const nlohmann::json& input) {
  auto& py = PolyglotAPI::Python::PythonEngine::instance();
  py.executeFile(input["Filename"]);
  return "";
}

std::string ScriptAPI::executeLuaDescription() {
  return R"(
calls a lua file

executePythonFile({
    'Filename' : 'main.lua'
});
)";
}

nlohmann::json ScriptAPI::executeLuaFile(const nlohmann::json& input) {
  auto lua = PolyglotAPI::Lua::LuaEngine();
  PolyglotAPI::FunctionRelay& relay = lua.getRelay();
  lua.addApi(database.api->getAPI(relay));
  lua.executeFile(input["Filename"]);
  return "";
}

std::string ScriptAPI::getDocumentationDescription() {
  return R"(
returns all descriptions of this api

getDocumentation({
});
)";
}

nlohmann::json ScriptAPI::getDocumentation(const nlohmann::json& input) {
  auto& py = PolyglotAPI::Python::PythonEngine::instance();
  auto api = database.api->getAPI(py.getRelay());

  std::string result;

  for (size_t i = 0; i < api->numberOfFunctions(); i++) {
    auto& fun = api->getFunction(i);
    result += "+++++++++++++++++++++++++\n";
    result += fun.getName() + "\n";
    result += "+++++++++++++++++++++++++\n";
    result += fun.getDescription() + "\n";    
  }

  return result;
}

std::string ScriptAPI::setDataDescription() {
  return R"(
sets custom data accessible by different files (e.g. executed with executePythonFile)
setData({
    'Name' : 'Config',
    'Data' : {'wub' : 'wub'}
});
)";
}

nlohmann::json ScriptAPI::setData(const nlohmann::json& input) {
  database.data[input["Name"]] = input["Data"];
  return "";
}

std::string ScriptAPI::getDataDescription() {
  return R"(
gets custom data set with setData
getData({
    'Name' : 'Config'
});
)";
}

nlohmann::json ScriptAPI::getData(const nlohmann::json& input) {
  return database.data[input["Name"]];
}


std::string ScriptAPI::setDirectoryDescription() {
  return R"(
sets the current directory for saving stuff and executing scripts

setDirectory({
    'Directory' : 'C:\\Hallo'
});
)";
}

nlohmann::json ScriptAPI::setDirectory(const nlohmann::json& input) {
  std::filesystem::current_path(input["Directory"]);
  return "";
}