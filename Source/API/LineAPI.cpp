#include "LineAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"

#include "GCode/LineMerger.h"


LineAPI::LineAPI(apiDatabase& db) : database(db) {

}

LineAPI::~LineAPI() {

}

void LineAPI::add(PolyglotAPI::API& api, PolyglotAPI::FunctionRelay& relay) {
  //delete lines
  std::unique_ptr<PolyglotAPI::APIFunction> deleteLinesAPI = std::make_unique<PolyglotAPI::APIFunction>("deleteLines", [this](const nlohmann::json& input) { return deleteLines(input); });
  deleteLinesAPI->setDescription(deleteLinesDescription());
  api.addFunction(std::move(deleteLinesAPI));
  //merge lines
  std::unique_ptr<PolyglotAPI::APIFunction> mergeLinesAPI = std::make_unique<PolyglotAPI::APIFunction>("mergeLines", [this](const nlohmann::json& input) { return mergeLines(input); });
  mergeLinesAPI->setDescription(mergeLinesDescription());
  api.addFunction(std::move(mergeLinesAPI));
  //get lines
  std::unique_ptr<PolyglotAPI::APIFunction> getLinesAPI = std::make_unique<PolyglotAPI::APIFunction>("getLines", [this](const nlohmann::json& input) { return getLines(input); });
  getLinesAPI->setDescription(getLinesDescription());
  api.addFunction(std::move(getLinesAPI));
}

std::string LineAPI::deleteLinesDescription() {
  return R"(
delete lines from memory

deleteLines({
    'Name' : 'Name'
});
)";
}

nlohmann::json LineAPI::deleteLines(const nlohmann::json& input) {
  database.lines.erase(input["Name"]);
  return "";
}

std::string LineAPI::mergeLinesDescription() {
  return R"(
merge lines to prohibit too much extruder hopping

mergeLines({
    'Result':'Name',
    'Radius' : 0.6,
    'Input' : ['Name1','Name2',...]
});
)";
}

nlohmann::json LineAPI::mergeLines(const nlohmann::json& input) {
  std::vector<std::vector<glm::dvec3>> target;
  for (auto& x : input["Input"]) {
    for (auto& p : *database.lines[x])
      target.push_back(p);
  }

  std::vector<std::vector<glm::dvec3>> result = LineMerger::merge(target, input["Radius"]);

  database.lines[input["Result"]] = std::make_unique<std::vector<std::vector<glm::dvec3>>>(result);
  return "";
}

std::string LineAPI::getLinesDescription() {
  return R"(
returns lines

result = getLines({
    'Name' : 'Name'
});
result = [[{0,0,0},{0,0,1},{0,0,2},...],[],[]]
)";
}

nlohmann::json LineAPI::getLines(const nlohmann::json& input) {
  auto& l = *database.lines[input["Name"]];
  nlohmann::json result = nlohmann::json::array();

  for (auto& streak : l) {
    nlohmann::json sub = nlohmann::json::array();

    for (auto& point : streak)
      sub.push_back({ point[0],point[1],point[2] });

    result.push_back(sub);
  }

  return result;
}