![image](https://github.com/Liech/FarfalleGCodeGen/assets/16963076/b692805e-53cc-418e-be39-0091a5623fb7)
![image](https://github.com/Liech/Farfalle/assets/16963076/ee5b53db-d740-4cae-b1f6-14f932f8427a)
![image](https://github.com/Liech/Farfalle/assets/16963076/6dd55645-5d1d-4bfa-b919-280fd44da71f)
![image](https://github.com/Liech/Farfalle/assets/16963076/ad9b9606-e378-4d03-9b52-6494d5518c83)
![image](https://github.com/Liech/Farfalle/assets/16963076/ef65e40f-aeb2-402b-a6e6-e2b9f377ca49)
![image](https://github.com/Liech/Farfalle/assets/16963076/65dfe39b-06e8-44b0-b824-0f57ff15acee)

# Farfalle

This is an experimental Slicer in a very rough state.

Orchestrated by a Python Script the executable produces gcode.

# Current Form

* Slice with implicit surfaces for infill and plane
* Slice implicit surfaces circular
* Voxel based Mesh manipulation/remeshing

# Finished Form

An basic Slicer that is capable of non-planar-slicing with zero steps.

* Generate Density field, that "follows" the horizontal walls
* Slice along the density field with implicit surface slicing
* obtain horizontal walls by planar parameterization and triangulated shortest paths on surface
* Some kind of Infill capability that im not sure of yet

# License

https://www.gnu.org/licenses/lgpl-3.0.html

# Dependencies

* CMAKE - Build Tool
* VCPKG - Package Manager
* CGAL - Main Workhorse
* eigen3 - CGAL wants this
* glm - Vector Library
* nlohmann::json - Config Load/Save
