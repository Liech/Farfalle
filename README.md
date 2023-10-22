# Farfalle

This is an experimental Slicer in a very rough state.

Orchestrated by a Python Script the executable produces gcode.

It uses Mesh based Models instead of Axis aligned Planes to cut.

It is not finished yet.

# Single Border cut and a Sphere as Slice Surface:
![image](https://github.com/Liech/Farfalle/assets/16963076/ee5b53db-d740-4cae-b1f6-14f932f8427a)

# Erosion based Border fill mode on a Z Plane Sliced Model:

![image](https://github.com/Liech/Farfalle/assets/16963076/6dd55645-5d1d-4bfa-b919-280fd44da71f)
![image](https://github.com/Liech/Farfalle/assets/16963076/ad9b9606-e378-4d03-9b52-6494d5518c83)

# X Plane Slicing of Z Plane Sliced Model
![image](https://github.com/Liech/Farfalle/assets/16963076/ef65e40f-aeb2-402b-a6e6-e2b9f377ca49)
![image](https://github.com/Liech/Farfalle/assets/16963076/65dfe39b-06e8-44b0-b824-0f57ff15acee)

# License

https://www.gnu.org/licenses/lgpl-3.0.html

# Use

* build with VCPKG + CMAKE
* create main.py at the same folder as the executeable
* fill main.py and get inspirations in example folder
* documentation.py dumps a script reference

# Dependencies

* CMAKE - Build Tool
* VCPKG - Package Manager
* CGAL - Main Workhorse
* eigen3 - CGAL wants this
* glm - Vector Library
* nlohmann::json - Config Load/Save
