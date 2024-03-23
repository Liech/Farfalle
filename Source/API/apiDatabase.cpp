#include "apiDatabase.h"

#include "FarfalleAPI.h"
#include "PolyglotAPI/API/API.h"
#include "PolyglotAPI/Lua/LuaEngine.h"
#include "PolyglotAPI/Lua/FunctionRelay.h"

#include "GCode/Printer.h"
#include "GCode/Movement.h"
#include "GCode/Extruder.h"
#include "GCode/Temperature.h"

apiDatabase::apiDatabase(FarfalleAPI* a) {
  api = a;
}

apiDatabase::~apiDatabase() {

}

void apiDatabase::initLua() {
  lua = std::make_unique<PolyglotAPI::Lua::LuaEngine>();
  PolyglotAPI::FunctionRelay& relay = lua->getRelay();
  lua->addApi(api->getAPI(relay));
}