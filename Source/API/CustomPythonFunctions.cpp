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
  //engine.addCustomFunction("GetFarfalleVoxel", [&db](pybind11::module_& m) {
  //  m.def("GetFarfalleVoxel", [&db](const pybind11::object& input) {
  //    PolyglotAPI::Python::FunctionRelay r;
  //    nlohmann::json inputJ = PolyglotAPI::Python::Conversion::py2j(input,r);
  //    if (!inputJ.is_string())
  //      throw std::runtime_error("need string :(");
  //    std::string name = inputJ;
  //    auto iterator = db.voxel[name]->begin();
  //    auto v = (*iterator);
  //    return pybind11::array_t<bool>{db.voxel[name]->size(), v, pybind11::cast(v)};
  //    }, pybind11::arg("input") = pybind11::none());
  //  });


  //py::array_t<int> get_indices(Group & g) {
  //  return py::array_t<int>{5, g.indices, py::cast(g)};
  //}
}