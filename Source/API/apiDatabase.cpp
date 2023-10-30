#include "apiDatabase.h"

#include "FarfalleAPI.h"
#include "Geometry/Model.h"
#include "PolyglotAPI/API/API.h"
#include "PolyglotAPI/Lua/LuaEngine.h"
#include "PolyglotAPI/Lua/FunctionRelay.h"

apiDatabase::apiDatabase(FarfalleAPI* a) {
  api = a;
  lua = std::make_unique<PolyglotAPI::Lua::LuaEngine>();
  PolyglotAPI::FunctionRelay& relay = lua->getRelay();
  lua->addApi(api->getAPI(relay));
}

apiDatabase::~apiDatabase() {

}