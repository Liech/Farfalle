#include "ScriptAPI.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"
#include "PolyglotAPI/Source/PolyglotAPI/Python/PythonEngine.h"
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
  //returns api documentation
  std::unique_ptr<PolyglotAPI::APIFunction> documentationAPI = std::make_unique<PolyglotAPI::APIFunction>("getDocumentation", [this](const nlohmann::json& input) { return getDocumentation(input); });
  documentationAPI->setDescription(getDocumentationDescription());
  api.addFunction(std::move(documentationAPI));
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