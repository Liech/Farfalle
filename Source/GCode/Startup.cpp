#include "Startup.h"

namespace GCode
{
  std::string Startup::toString() const
  {
    return
R"(
M107 ; Turn Fan Off
M190 S55 ; set bed temperature and wait for it to be reached
M104 S215 ; set extruder temperature 
print_start EXTRUDER=215 BED=55
M109 S215 ; set temperature and wait for it to be reached
G21 ; set units to millimeters
G90 ; use absolute coordinates
M83 ; use relative distances for extrusion
M107 ; Turn Fan Off
G92 E0 ; Set Extruder position to 0
G1 Z.2 F18000 ; Linear move to 0.2 with max speed of 18000
G1 E-2 F3000  ; Linear move to 0.2 with max speed of 3000
G1 Z.6 F18000 ; Linear move to 0.6 withmax speed of 18000
)";
  }
}