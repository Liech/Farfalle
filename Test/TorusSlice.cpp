#include <catch2/catch_test_macros.hpp>

#include "API/FarfalleAPI.h"
#include "PolyglotAPI/API/TestFunctionRelay.h"
#include "PolyglotAPI/API/API.h"

TEST_CASE("TorusSlice", "[TorusSlice]") {
  FarfalleAPI a;
  PolyglotAPI::TestFunctionRelay relay([](size_t id, const nlohmann::json&) { return nlohmann::json(); });
  auto api = a.getAPI(relay);

  //loadModel({ 'Name': 'Main', 'Filename' : Filename })
  auto loadModelParams = nlohmann::json::object({ {"Name", "Model"}, {"Filename", "TestData/torus.stl"} });
  api->getFunction("loadModel").call(loadModelParams);

  auto createPlaneParams = nlohmann::json::object({ 
    {"Name"  , "Tool"},
    {"Origin", {0,0,0}},
    {"Normal", {0,0,1}},
    {"Size"  , 10}
  });
  api->getFunction("createPlane").call(createPlaneParams);

  auto sliceModelParams = nlohmann::json::object({
    {"ModelName"  , "Model" },
    {"ToolName"   , "Tool"  },
    {"Mode"       , "Line"  },
    {"LineName"   , "Result"},
    });
  api->getFunction("sliceModel").call(sliceModelParams);

  auto getLineParams = nlohmann::json::object({{"Name" , "Result"}});
  auto result = api->getFunction("getLines").call(getLineParams);

  std::string dbg = "";
  for (auto& x : result) {
    dbg += "--------------------\n";
    for (auto& y : x) {
      dbg += y.dump() + "\n";
    }
  }

  REQUIRE(result.size() == 2);
}