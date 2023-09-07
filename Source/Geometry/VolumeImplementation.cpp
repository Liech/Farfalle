#include "VolumeImplementation.h"

#include <CGAL/boost/graph/convert_nef_polyhedron_to_polygon_mesh.h>


#include "Model.h"
#include "ModelImplementation.h"

VolumeImplementation::VolumeImplementation(Surface_Mesh& model) {
  nefRepresentation = std::make_unique<NefPolyhedron>(model);
}

VolumeImplementation::~VolumeImplementation() {

}

std::unique_ptr<Model> VolumeImplementation::getModel() const {
  std::unique_ptr<ModelImplementation> modelCore = std::make_unique<ModelImplementation>(); 
  CGAL::convert_nef_polyhedron_to_polygon_mesh(*nefRepresentation, modelCore->mesh, true);
  auto result = std::make_unique<Model>(std::move(modelCore));
  return std::move(result);
}