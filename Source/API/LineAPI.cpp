#include "LineAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"


LineAPI::LineAPI(apiDatabase& db) : database(db) {

}

LineAPI::~LineAPI() {

}

void LineAPI::add(PolyglotAPI::API& api, PolyglotAPI::FunctionRelay& relay) {
  //delete lines
  std::unique_ptr<PolyglotAPI::APIFunction> deleteLinesAPI = std::make_unique<PolyglotAPI::APIFunction>("deleteLines", [this](const nlohmann::json& input) { return deleteLines(input); });
  deleteLinesAPI->setDescription(deleteLinesDescription());
  api.addFunction(std::move(deleteLinesAPI));
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