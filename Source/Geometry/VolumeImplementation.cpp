#include "VolumeImplementation.h"

#include <CGAL/boost/graph/convert_nef_polyhedron_to_polygon_mesh.h>
#include <CGAL/minkowski_sum_3.h>
#include <CGAL/convex_decomposition_3.h>
#include <CGAL/Cartesian_converter.h>
#include <CGAL/number_utils.h>

#include "Model.h"
#include "ModelImplementation.h"

typedef CGAL::Lazy_exact_nt<mpq_class> Exact;
//typedef CGAL::to_double Inexact;

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

  std::map<CGAL::SM_Vertex_index, CGAL::SM_Vertex_index> vMap;
  for (auto& vertex : input.vertices()) {
    Point_3& point = input.point(vertex);
    EPoint_3 epoint(Exact(point.x()), Exact(point.y()), Exact(point.z()));
    auto newVertex = result->add_vertex(epoint);
    vMap[vertex] = newVertex;
  }
  for (auto& face : input.faces()) {
    auto start = input.halfedge(face);
    std::vector<CGAL::SM_Vertex_index> indices;
    auto current = input.next(start);
    indices.push_back(input.source(start));
    while(current != start){
      indices.push_back(input.source(current));
      current = input.next(current);
    }
    result->add_face(indices);
  }
  return std::move(result);
}


std::unique_ptr<Surface_Mesh> VolumeImplementation::convertBack(ESurface_Mesh& input) const {
  std::unique_ptr<Surface_Mesh> result = std::make_unique<Surface_Mesh>();

  std::map<CGAL::SM_Vertex_index, CGAL::SM_Vertex_index> vMap;
  for (auto& vertex : input.vertices()) {
    EPoint_3& epoint = input.point(vertex);
    Point_3 point(CGAL::to_double(epoint.x()), CGAL::to_double(epoint.y()), CGAL::to_double(epoint.z()));
    auto newVertex = result->add_vertex(point);
    vMap[vertex] = newVertex;
  }
  for (auto& face : input.faces()) {
    auto start = input.halfedge(face);
    std::vector<CGAL::SM_Vertex_index> indices;
    auto current = input.next(start);
    indices.push_back(input.source(start));
    while (current != start) {
      indices.push_back(input.source(current));
      current = input.next(current);
    }
    result->add_face(indices);
  }
  return std::move(result);
}