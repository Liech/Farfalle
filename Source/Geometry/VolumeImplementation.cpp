#include "VolumeImplementation.h"

#include <CGAL/boost/graph/convert_nef_polyhedron_to_polygon_mesh.h>
#include <CGAL/minkowski_sum_3.h>
#include <CGAL/convex_decomposition_3.h>
#include <CGAL/Cartesian_converter.h>




#include "Model.h"
#include "ModelImplementation.h"

typedef CGAL::Lazy_exact_nt<mpq_class> Exact;

VolumeImplementation::VolumeImplementation(Surface_Mesh& model) {
  std::unique_ptr<ESurface_Mesh> emodel = convert(model);
  nefRepresentation = std::make_unique<NefPolyhedron>(*emodel);
  //CGAL::convex_decomposition_3(*nefRepresentation);
}

VolumeImplementation::~VolumeImplementation() {

}

std::unique_ptr<Model> VolumeImplementation::getModel() const {
  std::unique_ptr<ModelImplementation> modelCore = std::make_unique<ModelImplementation>(); 
  ESurface_Mesh emesh;
  CGAL::convert_nef_polyhedron_to_polygon_mesh(*nefRepresentation, emesh, true);
  auto mesh = convertBack(emesh);
  modelCore->mesh = *mesh;
  auto result = std::make_unique<Model>(std::move(modelCore));
  return std::move(result);
}

void VolumeImplementation::minkowski(VolumeImplementation& tool) {
  auto result = CGAL::minkowski_sum_3(*nefRepresentation, *tool.nefRepresentation);
  nefRepresentation = std::make_unique< NefPolyhedron>(result);
}

std::unique_ptr<ESurface_Mesh> VolumeImplementation::convert(Surface_Mesh& input) const {
  std::unique_ptr<ESurface_Mesh> result = std::make_unique<ESurface_Mesh>();
  throw std::runtime_error("Not implemented yet!");

  std::map<CGAL::SM_Vertex_index, CGAL::SM_Vertex_index> vMap;
  for (auto& vertex : input.vertices()) {
    Point_3& point = input.point(vertex);
    EPoint_3 epoint(Exact(point.x()), Exact(point.y()), Exact(point.z()));
    auto newVertex = result->add_vertex(epoint);
    vMap[vertex] = newVertex;
  }
  for (auto& face : input.faces()) {
    auto faceEdge = input.halfedge(face);
    std::vector<CGAL::SM_Vertex_index> indices;

    //emesh.add_face()
  }
  return nullptr;
}


std::unique_ptr<Surface_Mesh> VolumeImplementation::convertBack(ESurface_Mesh& input) const {
  throw std::runtime_error("Not implemented yet!");
  return nullptr;
}