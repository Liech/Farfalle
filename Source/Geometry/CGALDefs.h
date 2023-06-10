#pragma once

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/boost/graph/helpers.h>
#include <CGAL/Polygon_mesh_slicer.h>
#include "CGAL/boost/graph/convert_nef_polyhedron_to_polygon_mesh.h"
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/convex_hull_3.h>
#include <CGAL/Polygon_mesh_processing/intersection.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>

#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/IO/Nef_polyhedron_iostream_3.h>
#include <CGAL/minkowski_sum_3.h>

#include <CGAL/Polygon_mesh_processing/clip.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/Complex_2_in_triangulation_3.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Implicit_surface_3.h>
#include <CGAL/IO/facets_in_complex_2_to_triangle_mesh.h>
#include <CGAL/Surface_mesh_parameterization/parameterize.h>
#include <CGAL/Polygon_mesh_processing/measure.h>

namespace PMP = CGAL::Polygon_mesh_processing;

typedef CGAL::Exact_predicates_inexact_constructions_kernel       Kernel;
typedef Kernel::Point_3                                           Point;
typedef CGAL::Surface_mesh<Point>                                 Surface_mesh;
typedef std::vector<Kernel::Point_3>                              Polyline_type;
typedef std::list<Polyline_type>                                  Polylines;
typedef CGAL::AABB_halfedge_graph_segment_primitive<Surface_mesh> HGSP;
typedef CGAL::AABB_traits<Kernel, HGSP>                           AABB_traits;
typedef CGAL::AABB_tree<AABB_traits>                              AABB_tree;
typedef CGAL::Nef_polyhedron_3<Kernel, CGAL::SNC_indexed_items>   Nef_polyhedron;
typedef CGAL::Polyhedron_3<Kernel>                                Polyhedron;
typedef boost::graph_traits<Surface_mesh>::halfedge_descriptor    halfedge_descriptor;
typedef boost::graph_traits<Surface_mesh>::vertex_descriptor      vertex_descriptor;
typedef CGAL::Surface_mesh_default_triangulation_3 Tr;
typedef CGAL::Complex_2_in_triangulation_3<Tr> C2t3;
typedef Tr::Geom_traits GT;
typedef GT::FT FT;
typedef FT(*Function)(Point);
typedef CGAL::Implicit_surface_3<GT, Function> Surface_3;
typedef GT::Sphere_3 Sphere;
namespace SMP = CGAL::Surface_mesh_parameterization;

typedef Kernel::Point_2 Point_2;
typedef Surface_mesh::Property_map<vertex_descriptor, Point_2>  UV_pmap;