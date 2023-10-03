#pragma once

namespace PolyglotAPI {
  class API;
  class FunctionRelay;
}

class FarfalleAPI {
public:
  void add(PolyglotAPI::API&, PolyglotAPI::FunctionRelay& relay);
};