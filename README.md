# Farfalle

This is an experimental Slicer in a very rough state.

Orchestrated by a Python or Lua Script the executable produces gcode.

It uses Mesh based Models instead of Axis aligned Planes to cut.

Therefore it might be a little bit slower than what you are used too. But it is capable of pretty neat stuff:

 * Non-Planar Printing
 * STL Mesh Based Infill Patterns
 * Function Defined Infill
 * Nonplanar Build Plate

Also the script code is pretty small and therefor hopefully inviting for experimentation and learning how slicers work.

It is not finished yet though.

# Planar Benchy

![image](https://github.com/Liech/Farfalle/assets/16963076/0a7804f8-7165-44d1-a22d-eb7d4a5f5a29)
![image](https://github.com/Liech/Farfalle/assets/16963076/3175d62c-1d4b-4b9d-95bc-18cf9a1bb011)
There are Gaps between the lines, because the extrusion is not yet tuned. Im currently concentrating on the correct toolpath.


# Non-Planar Benchy

![image](https://github.com/Liech/Farfalle/assets/16963076/6f8c8b6b-6ea7-4a43-891e-270a4c3548f2)
![image](https://github.com/Liech/Farfalle/assets/16963076/31e5ac3a-adde-4039-9acf-06baad6e3da8)
![image](https://github.com/Liech/Farfalle/assets/16963076/f270d1fe-ab3e-4424-821b-30a7f16e2c2b)

Obviously there is some tuning of the retraction settings needed.

# Future

## Next Steps

 * It is currently pretty slow. Slicing a Benchy can take a day. I am confident that it would be possible in under 15 Minutes.
 * The Retraction Settings and similar Settings are not tuned, therefor a lot of stringing is happening
 * The line Merging is naive and it should incorporate also wall sections.
 * The order the lines are printed does not respect common sense
 * The drawn lines are too detailed. It should be possible to shrink down the slicer files.

## Far Future

 * Instead of Sinus Curves it should be possible to follow top Surfaces
 * Infill
 * Support
 * Mindfullness regarding Printer Geometry
 * 3D Non Planar Polygon Offsetting instead of Voxel Erosion
 * Multi Layer Region Coupling to reduce stringing potential
 * I want to experiment with intentionally move the nozzle into the already printed material to stich layers together
 * I want to experiment with Layerless Hot Plasticblob as Infill to get stronger prints

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
