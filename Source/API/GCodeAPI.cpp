#include "GCodeAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"

#include "Tools/String2File.h"
#include "GCode/LinearPrint.h"
#include "GCode/Printer.h"
#include "GCode/Travel.h"

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

  //set heat
  std::unique_ptr<PolyglotAPI::APIFunction> seatHeatAPI = std::make_unique<PolyglotAPI::APIFunction>("setHeat", [this](const nlohmann::json& input) { return setHeat(input); });
  seatHeatAPI->setDescription(setHeatDescription());
  api.addFunction(std::move(seatHeatAPI));

  //shutdown
  std::unique_ptr<PolyglotAPI::APIFunction> shutdownAPI = std::make_unique<PolyglotAPI::APIFunction>("shutdown", [this](const nlohmann::json& input) { return shutdown(input); });
  shutdownAPI->setDescription(shutdownDescription());
  api.addFunction(std::move(shutdownAPI));

  //startup
  std::unique_ptr<PolyglotAPI::APIFunction> startupAPI = std::make_unique<PolyglotAPI::APIFunction>("startup", [this](const nlohmann::json& input) { return startup(input); });
  startupAPI->setDescription(shutdownDescription());
  api.addFunction(std::move(startupAPI));

  //prime
  std::unique_ptr<PolyglotAPI::APIFunction> primeAPI = std::make_unique<PolyglotAPI::APIFunction>("prime", [this](const nlohmann::json& input) { return prime(input); });
  primeAPI->setDescription(primeDescription());
  api.addFunction(std::move(primeAPI));
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
  if (!database.lines.contains(input["Line"]))
    throw std::runtime_error("Line with name " + (std::string)input["Line"] + " not found!");
  for (auto& streak : *database.lines[input["Line"]]) {
    gcode.controlPoints = streak;
    gcode.feedrate = 0.03;

    GCode::Travel travel(*database.printer);
    travel.controlPoints = { database.printer->movement->currentPosition + glm::dvec3(0,0,1) ,glm::dvec3(streak[0][0], streak[0][1],database.printer->movement->currentPosition[2] + 1), streak[0] };

    //database.printer->extruder->retract(result);
    database.printer->movement->travelMode(result);
    travel.toString(result);
    database.printer->extruder->unretract(result);
    database.printer->movement->printMode(result);
    gcode.toString(result);
    database.printer->extruder->retract(result);
  }
  return result;
}


std::string GCodeAPI::setHeatDescription() {
  return R"(
returns text setting the printer heat level.
Mode Heat sets heats
Mode WaitForHeat sets heat and waits
Mode Set sets heat, but do not commuicate to the printer (e.g. before startup)

setHeat({
    'Mode' : 'Wait', # Heat/WaitForHeat/Set
    'Nozzle' : 215,
    'Bed': 60
});
)";
}

nlohmann::json GCodeAPI::setHeat(const nlohmann::json& input) {
  std::string mode = input["Mode"];
  database.printer->temperature->bed = input["Bed"];
  database.printer->temperature->extruder = input["Nozzle"];
  std::string result;
  if (mode == "WaitForHeat")
    database.printer->temperature->waitForHeating(result);
  else if (mode == "Heat")
    database.printer->temperature->startHeating(result);
  else
    result = "";
  return result;
}

std::string GCodeAPI::shutdownDescription() {
  return R"(
returns gcode shutting down the printer to a save state

shutdown({});
)";
}

nlohmann::json GCodeAPI::shutdown(const nlohmann::json& input) {
  std::string result;
  database.printer->shutdown(result);
  return result;
}

std::string GCodeAPI::startupDescription() {
  return R"(
returns gcode starting up the printer

startup({});
)";
}

nlohmann::json GCodeAPI::startup(const nlohmann::json& input) {
  std::string result;
  database.printer->startup(result);
  return result;
}

std::string GCodeAPI::primeDescription() {
  return R"(
returns gcode priming the nozzle

prime({});
)";
}

nlohmann::json GCodeAPI::prime(const nlohmann::json& input) {
  std::string result;
  database.printer->extruder->prime(result);
  return result;
}