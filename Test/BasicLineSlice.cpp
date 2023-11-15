#include <catch2/catch_test_macros.hpp>

#include "API/FarfalleAPI.h"
#include "PolyglotAPI/API/TestFunctionRelay.h"
#include "PolyglotAPI/API/API.h"

TEST_CASE("BasicLineSlice", "[BasicLineSlice]") {
  FarfalleAPI a;
  PolyglotAPI::TestFunctionRelay relay([](size_t id, const nlohmann::json&) { return nlohmann::json(); });
  auto api = a.getAPI(relay);

  auto createPlaneParams = nlohmann::json::object({ 
    {"Name"  , "Slice"},
    {"Origin", {0,0,1}},
    {"Normal", {0,0,1}},
    {"Size"  , 1}
  });
  api->getFunction("createPlane").call(createPlaneParams);

  auto createToolParams = nlohmann::json::object({
    {"Name"  , "Tool"},
    {"Origin", {0,0,1}},
    {"Normal", {1,0,0}},
    {"Size"  , 2}
    });
  api->getFunction("createPlane").call(createToolParams);

  auto sliceModelParams = nlohmann::json::object({
    {"ModelName"  , "Slice" },
    {"ToolName"   , "Tool"  },
    {"Mode"       , "Line"  },
    {"LineName"   , "Result"},
    });
  api->getFunction("sliceModel").call(sliceModelParams);

  auto getLineParams = nlohmann::json::object({
    {"Name"  , "Result"},
    });
  auto result = api->getFunction("getLines").call(getLineParams);

  REQUIRE(result.size() == 1);
  REQUIRE(result[0].size() == 3);
  REQUIRE(result[0][0].size() == 3);
  REQUIRE(result[0][1].size() == 3);
  REQUIRE(result[0][2].size() == 3);
}