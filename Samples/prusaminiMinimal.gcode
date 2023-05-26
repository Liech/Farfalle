M201 X2500 Y2500 Z400 E5000 ; sets maximum accelerations, mm/sec^2
M203 X180 Y180 Z12 E80 ; sets maximum feedrates, mm / sec
M204 P2000 R1250 T2500 ; sets acceleration (P, T) and retract acceleration (R), mm/sec^2
M205 X8.00 Y8.00 Z2.00 E10.00 ; sets the jerk limits, mm/sec
M205 S0 T0 ; sets the minimum extruding and travel feed rate, mm/sec
M107
;TYPE:Custom
M862.3 P "MINI" ; printer model check
G90 ; use absolute coordinates
M83 ; extruder relative mode
M104 S170 ; set extruder temp for bed leveling
M140 S60 ; set bed temp
M109 R170 ; wait for bed leveling temp
M190 S60 ; wait for bed temp
M204 T1250 ; set travel acceleration
G28 ; home all without mesh bed level
G29 ; mesh bed leveling 
M204 T2500 ; restore travel acceleration
M104 S215 ; set extruder temp
G92 E0
G1 Y-2 X179 F2400
G1 Z3 F720
M109 S215 ; wait for extruder temp

; intro line
M73 P20 R0
G1 X170 F1000
M73 P21 R0
G1 Z0.2 F720
M73 P23 R0
G1 X110 E8 F900
M73 P24 R0
G1 X40 E10 F700
G92 E0

M221 S95 ; set flow
G21 ; set units to millimeters
G90 ; use absolute coordinates
M83 ; use relative distances for extrusion
M900 K0.2 ; Filament gcode LA 1.5
; ; Filament gcode LA 1.0
M107
;LAYER_CHANGE
;Z:0.2
;HEIGHT:0.2
;BEFORE_LAYER_CHANGE
G92 E0.0
;0.2


M73 P42 R0
G1 E-3.2 F4200
M73 P69 R0
G1 Z.2 F720
;AFTER_LAYER_CHANGE
;0.2
; printing object Form-Kubus id:0 copy 0
M73 P70 R0
G1 Z.4
G1 X80.2 Y90 F9000
G1 Z.2 F720
M73 P73 R0
G1 E3.2 F2400
M204 P600
;TYPE:External perimeter
;WIDTH:0.4
G1 F900
M73 P74 R0
G1 X99.8 Y90 E.58195
M204 P1000
; stop printing object Form-Kubus id:0 copy 0
G1 E-2.24 F4200
;WIPE_START
M73 P79 R0
G1 F7200;_WIPE
M73 P80 R0
G1 X98.154 Y90 E-.912
;WIPE_END
G1 E-.048 F4200
G1 Z.4 F720
M107

G1 E-1 F2100 ; retract
G1 Z2.2 F720 ; Move print head up
G1 X178 Y178 F4200 ; park print head
M73 P81 R0
G1 Z30.2 F720 ; Move print head further up
G4 ; wait
M104 S0 ; turn off temperature
M140 S0 ; turn off heatbed
M107 ; turn off fan
M221 S100 ; reset flow
M900 K0 ; reset LA
M84 ; disable motors
M73 P100 R0
