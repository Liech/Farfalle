#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "apiDatabase.h"

class VoxelAPI;
class ModelAPI;

namespace PolyglotAPI {
  class API;
  class FunctionRelay;
}

class FarfalleAPI {
public:
  FarfalleAPI();
  virtual ~FarfalleAPI();

  void add(PolyglotAPI::API&, PolyglotAPI::FunctionRelay& relay);

private:
  std::unique_ptr<VoxelAPI> voxel;
  std::unique_ptr<ModelAPI> model;
  apiDatabase db;
};