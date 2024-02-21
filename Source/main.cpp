#include <iostream>
#include <functional>
#include <stdexcept>

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include "PolyglotAPI/Python/PythonEngine.h"
#include "PolyglotAPI/Lua/LuaEngine.h"
#include "PolyglotAPI/API/API.h"
#include "API/FarfalleAPI.h"
#include <fstream>
#include <filesystem>

void enforceWorkingDir(std::string exeDir) {
  const size_t last_slash_idx = exeDir.find_last_of("\\/");
  if (std::string::npos != last_slash_idx) {
    exeDir.erase(last_slash_idx + 1);
  }
  std::filesystem::current_path(exeDir);
}

int main(int argc, char** argv) {
  //slice();
  enforceWorkingDir(std::string(argv[0]));
  

  FarfalleAPI farfalle;
  auto& py = PolyglotAPI::Python::PythonEngine::instance();
  std::shared_ptr<PolyglotAPI::API> api = farfalle.getAPI(py.getRelay());
  py.addAPI(api);
  farfalle.addCustomPythonFunctions();
  py.initialize();

  std::string current = argv[0];
  std::string fullpath ="main.py"; 
  bool lua = false;
  if (!std::filesystem::exists(fullpath)) {
    fullpath = "main.lua";
    lua = true;
  }
  std::cout << fullpath << std::endl;
  std::ifstream t(fullpath);
  std::string s((std::istreambuf_iterator<char>(t)),
    std::istreambuf_iterator<char>());
  std::cout << s << std::endl;
  std::cout << " --------execution start------- " << std::endl;
  try {
    if (lua)
      farfalle.getDB().lua->executeString(s);
    else
      py.execute(s);
  }
  catch (std::exception& e){
    std::cout << " --------error message ------- " << std::endl;
    std::cout << e.what() << std::endl;
    std::cout << " --------error message end ---------- " << std::endl;
  }
  std::cout << " --------execution end--------- " << std::endl;


  py.dispose();
}