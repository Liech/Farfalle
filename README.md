# Farfalle

This is an experimental Slicer in a very rough state.

Orchestrated by a Python or Lua Script the executable produces gcode.

It uses Mesh based Models instead of Axis aligned Planes to cut.

Therefore it might be a little bit slower than what you are used too. But it is capable of pretty neat stuff:

 * Non-Planar Printing
 * STL Mesh Based Infill Patterns
 * Function Defined Infill
 * Nonplanar Build Plate

It is not finished yet though.

# First Layer of a Benchy:

![image](https://github.com/Liech/Farfalle/assets/16963076/cfbc93d1-0b2c-4d8a-aefe-3cc99a688246)

There are Gaps between the lines, because the extrusion is not yet tuned. Im currently concentrating on the correct toolpath.

# License

https://www.gnu.org/licenses/lgpl-3.0.html

# Use

* make sure to pull submodules too
* build with VCPKG + CMAKE
* create main.py or main.lua at the same folder as the executeable
* fill main and get inspirations in example folder
* documentation.py dumps a script reference

# Dependencies

* CMAKE - Build Tool
* VCPKG - Package Manager
* CGAL - Main Workhorse
* eigen3 - CGAL wants this
* glm - Vector Library
* nlohmann::json - Config Load/Save
* Polyglot - My lib for scripting languages (Python & Lua)
