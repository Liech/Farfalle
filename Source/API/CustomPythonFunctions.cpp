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
  engine.addCustomFunction("GetFarfalleVoxel", [&db](pybind11::module_& m) {
    m.def("GetFarfalleVoxel", [&db](const pybind11::object& input) {
      auto& engine = PolyglotAPI::Python::PythonEngine::instance();
      nlohmann::json inputJ = PolyglotAPI::Python::Conversion::py2j(input,(PolyglotAPI::Python::FunctionRelay&)engine.getRelay());
      if (!inputJ.is_string())
        throw std::runtime_error("need string :(");
      std::string name = inputJ;
      auto vox = db.voxel[name].get();
      glm::ivec3 resolution = db.voxelResolution[name];
      size_t dataSize = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
      pybind11::ssize_t size = dataSize;
      auto shape = pybind11::array::ShapeContainer({ (pybind11::ssize_t)resolution.x, (pybind11::ssize_t)resolution.y, (pybind11::ssize_t)resolution.y });
      return pybind11::array_t<bool>{shape, vox, pybind11::cast(vox)};
      }, pybind11::arg("input") = pybind11::none());
    });


  //py::array_t<int> get_indices(Group & g) {
  //  return py::array_t<int>{5, g.indices, py::cast(g)};
  //}
}