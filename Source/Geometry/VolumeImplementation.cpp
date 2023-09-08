#include "VolumeImplementation.h"

#include <CGAL/boost/graph/convert_nef_polyhedron_to_polygon_mesh.h>
#include <CGAL/minkowski_sum_3.h>
#include <CGAL/convex_decomposition_3.h>
//#include <CGAL/Cartesian_converter.h>




#include "Model.h"
#include "ModelImplementation.h"


VolumeImplementation::VolumeImplementation(Surface_Mesh& model) {
  //nefRepresentation = std::make_unique<NefPolyhedron>(model);
  //CGAL::convex_decomposition_3(*nefRepresentation);
}

VolumeImplementation::~VolumeImplementation() {

}

std::unique_ptr<Model> VolumeImplementation::getModel() const {
  std::unique_ptr<ModelImplementation> modelCore = std::make_unique<ModelImplementation>(); 
  //CGAL::convert_nef_polyhedron_to_polygon_mesh(*nefRepresentation, modelCore->mesh, true);
  auto result = std::make_unique<Model>(std::move(modelCore));
  return std::move(result);
}

void VolumeImplementation::minkowski(VolumeImplementation& tool) {
  auto result = CGAL::minkowski_sum_3(*nefRepresentation, *tool.nefRepresentation);
  nefRepresentation = std::make_unique< NefPolyhedron>(result);
}