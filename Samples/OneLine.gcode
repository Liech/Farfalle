
M107
M190 S55 ; set bed temperature and wait for it to be reached
M104 S215 ; set temperature
print_start EXTRUDER=215 BED=55
M109 S215 ; set temperature and wait for it to be reached
G21 ; set units to millimeters
G90 ; use absolute coordinates
M83 ; use relative distances for extrusion
M107
G92 E0

G1 Z.2 F18000
G1 E-2 F3000
G1 Z.6 F18000
G1 X60 Y61.3
G1 Z.2
G1 E2 F1800
M204 S1000
G1 F900
G1 X60 Y58.7 E.0772
M204 S3000
G1 E-2 F3000
G1 Z.6 F18000
M107
print_end    ;end script from macro