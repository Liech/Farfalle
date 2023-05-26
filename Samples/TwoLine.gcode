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
G1 X60 Y69.8  ;travel height-> print height
G1 Z.2        ;travel height-> print height
G1 E2 F1800   ;travel height-> print height
M204 S1000    ;print acceleration
G1 F900                    ;Print
G1 X60 Y50.2 E.58195       ;Print
G1 X79.638 Y50.132 E.58195 ; Print
M204 S3000                 ;Retraction
G1 E-2 F3000               ;Retraction
G1 Z.6 F18000              ;Retraction
M107
print_end    ;end script from macro