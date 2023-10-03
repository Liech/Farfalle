#include "FarfalleAPI.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "PolyglotAPI/Source/PolyglotAPI/API/API.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/APIFunction.h"
#include "PolyglotAPI/Source/PolyglotAPI/API/FunctionRelay.h"

void FarfalleAPI::add(PolyglotAPI::API& api, PolyglotAPI::FunctionRelay& relay) {
  std::unique_ptr<PolyglotAPI::APIFunction> create = std::make_unique<PolyglotAPI::APIFunction>("voxivoxvox", [&relay, this](const nlohmann::json& input) {
    std::cout << "Doing great stuff" << std::endl;

    return input;
    });
  create->setDescription(R"(
Great description
)");
  api.addFunction(std::move(create));
}