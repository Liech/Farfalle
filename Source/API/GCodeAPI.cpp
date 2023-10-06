#include "GCodeAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"

#include "Tools/String2File.h"
#include "GCode/LinearPrint.h"
#include "GCode/Printer.h"

GCodeAPI::GCodeAPI(apiDatabase& db) : database(db) {
  db.printer = std::make_unique<Printer>();
}

GCodeAPI::~GCodeAPI() {

}

void GCodeAPI::add(PolyglotAPI::API& api, PolyglotAPI::FunctionRelay& relay) {

  //save text
  std::unique_ptr<PolyglotAPI::APIFunction> saveTextAPI = std::make_unique<PolyglotAPI::APIFunction>("saveText", [this](const nlohmann::json& input) { return saveText(input); });
  saveTextAPI->setDescription(saveTextDescription());
  api.addFunction(std::move(saveTextAPI));

  //linear print
  std::unique_ptr<PolyglotAPI::APIFunction> linearPrintAPI = std::make_unique<PolyglotAPI::APIFunction>("linearPrint", [this](const nlohmann::json& input) { return linearPrint(input); });
  linearPrintAPI->setDescription(linearPrintDescription());
  api.addFunction(std::move(linearPrintAPI));
}

std::string GCodeAPI::saveTextDescription() {
  return R"(
save text to file

saveText({
    'Text' : 'Hello World!',
    'Filename' : 'pretty.gcode'
});
)";
}

nlohmann::json GCodeAPI::saveText(const nlohmann::json& input) {
  Tools::String2File(input["Filename"], input["Text"]);
  return "";
}

std::string GCodeAPI::linearPrintDescription() {
  return R"(
lines to gcode

linearPrint({
    'Line' : 'HalfBakedGCode',
    'Feedrate' : 0.03
});
)";
}

nlohmann::json GCodeAPI::linearPrint(const nlohmann::json& input) {
  std::string result;
  GCode::LinearPrint gcode = GCode::LinearPrint(*database.printer);
  for (auto& streak : *database.lines[input["Line"]]) {
    gcode.controlPoints = streak;
    gcode.feedrate = 0.03;
    gcode.toString(result);
  }
  return result;
}