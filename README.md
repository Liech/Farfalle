![image](https://github.com/Liech/FarfalleGCodeGen/assets/16963076/b692805e-53cc-418e-be39-0091a5623fb7)
![image](https://github.com/Liech/Farfalle/assets/16963076/ee5b53db-d740-4cae-b1f6-14f932f8427a)

# Farfalle

This is an experimental Slicer in a very rough state.

# Current Form

It can only generate vertical walls. It can does this either with planar slicing or with implicit surface slicing.

# Finished Form

An basic Slicer that is capable of non-planar-slicing with zero steps.

* Generate Density field, that "follows" the horizontal walls
* Slice along the density field with implicit surface slicing
* obtain horizontal walls by planar parameterization and triangulated shortest paths on surface
* Some kind of Infill capability that im not sure of yet

# Dependencies

* CMAKE - Build Tool
* VCPKG - Package Manager
* CGAL - Main Workhorse
* eigen3 - CGAL wants this
* glm - Vector Library
* nlohmann::json - Config Load/Save
