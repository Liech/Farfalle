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