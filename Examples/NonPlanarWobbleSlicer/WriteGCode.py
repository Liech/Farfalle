print('Write GCode');

config      = getData({'Name':'config'});
ResultFilename = config['ResultFilename'];


gcode = ""

gcode += setHeat({
    'Mode' : 'Set',
    'Nozzle' : 215,
    'Bed': 60
});

gcode += startup({});
gcode += prime({});

gcode += "\n; ;;;;;;;;;;;;;;;;;;";
gcode += "\n; Printing:  ";
gcode += "\n; ;;;;;;;;;;;;;;;;;;\n";


zCounter = getData({'Name':'ZCounter'});

for i in range(0,zCounter):
  gcode += linearPrint({
    'Line': 'Result' + str(i),
    'Feedrate': 0.025
  })

gcode += shutdown({});

saveText({
    'Text' : gcode,
    'Filename' : ResultFilename
});


