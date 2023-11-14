#include <catch2/catch_test_macros.hpp>

#include "API/FarfalleAPI.h"
#include "PolyglotAPI/API/TestFunctionRelay.h"
#include "PolyglotAPI/API/API.h"

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}

TEST_CASE("FarfalleAPI Access", "[api]") {
  FarfalleAPI a;
  PolyglotAPI::TestFunctionRelay relay([](size_t id, const nlohmann::json&) { return nlohmann::json(); });
  auto api = a.getAPI(relay);

  auto createPlaneParams = nlohmann::json::object({ 
    {"Name"  , "Slice"},
    {"Origin", {0,0,1}},
    {"Normal", {0,0,1}},
    {"Size"  , 400}
  });
  api->getFunction("createPlane").call(createPlaneParams);

  REQUIRE(true);
}