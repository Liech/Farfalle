#include "CustomPythonFunctions.h"

#include "PolyglotAPI/Python/PythonEngine.h"
#include "PolyglotAPI/Python/Conversion.h"
#include "PolyglotAPI/Python/FunctionRelay.h"

#include "apiDatabase.h"

#include <python3.10/Python.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/numpy.h>


void CustomPythonFunctions::add(apiDatabase& db) {
  auto& engine = PolyglotAPI::Python::PythonEngine::instance();
  engine.addCustomFunction("Hallo", [](pybind11::module_& m) {
    m.def("Hallo", [](const pybind11::object& input) {
      return input;
      }, pybind11::arg("input") = pybind11::none());
    });
  engine.addCustomFunction("GetFarfalleBool", [&db](pybind11::module_& m) {
    m.def("GetFarfalleBool", [&db](const pybind11::object& input) {
      auto& engine = PolyglotAPI::Python::PythonEngine::instance();
      nlohmann::json inputJ = PolyglotAPI::Python::Conversion::py2j(input, (PolyglotAPI::Python::FunctionRelay&)engine.getRelay());
      if (!inputJ.is_string())
        throw std::runtime_error("need string :(");
      std::string name = inputJ;
      auto vox = db.boolField[name].first.get();
      glm::ivec3 resolution = db.boolField[name].second;
      size_t dataSize = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
      pybind11::ssize_t size = dataSize;
      auto shape = pybind11::array::ShapeContainer({ (pybind11::ssize_t)resolution.x, (pybind11::ssize_t)resolution.y, (pybind11::ssize_t)resolution.y });
      return pybind11::array_t<bool>{shape, vox, pybind11::cast(vox)};
      }, pybind11::arg("input") = pybind11::none());
    });
  engine.addCustomFunction("GetFarfalleDouble", [&db](pybind11::module_& m) {
    m.def("GetFarfalleDouble", [&db](const pybind11::object& input) {
      auto& engine = PolyglotAPI::Python::PythonEngine::instance();
      nlohmann::json inputJ = PolyglotAPI::Python::Conversion::py2j(input, (PolyglotAPI::Python::FunctionRelay&)engine.getRelay());
      if (!inputJ.is_string())
        throw std::runtime_error("need string :(");
      std::string name = inputJ;
      auto vox = db.doubleField[name].first.get()->data();
      glm::ivec3 resolution = db.doubleField[name].second;
      size_t dataSize = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
      pybind11::ssize_t size = dataSize;
      auto shape = pybind11::array::ShapeContainer({ (pybind11::ssize_t)resolution.x, (pybind11::ssize_t)resolution.y, (pybind11::ssize_t)resolution.y });
      return pybind11::array_t<double>{shape, vox, pybind11::cast(vox)};
      }, pybind11::arg("input") = pybind11::none());
    });
  engine.addCustomFunction("GetFarfalleInt", [&db](pybind11::module_& m) {
    m.def("GetFarfalleInt", [&db](const pybind11::object& input) {
      auto& engine = PolyglotAPI::Python::PythonEngine::instance();
      nlohmann::json inputJ = PolyglotAPI::Python::Conversion::py2j(input, (PolyglotAPI::Python::FunctionRelay&)engine.getRelay());
      if (!inputJ.is_string())
        throw std::runtime_error("need string :(");
      std::string name = inputJ;
      auto vox = db.intField[name].first.get()->data();
      glm::ivec3 resolution = db.intField[name].second;
      size_t dataSize = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
      pybind11::ssize_t size = dataSize;
      auto shape = pybind11::array::ShapeContainer({ (pybind11::ssize_t)resolution.x, (pybind11::ssize_t)resolution.y, (pybind11::ssize_t)resolution.y });
      return pybind11::array_t<int32_t>{shape, vox, pybind11::cast(vox)};
      }, pybind11::arg("input") = pybind11::none());
    });


  //py::array_t<int> get_indices(Group & g) {
  //  return py::array_t<int>{5, g.indices, py::cast(g)};
  //}
}