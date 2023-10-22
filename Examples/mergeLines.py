filename = "3DBenchy.stl";

BuildplateCenter = [100,100,0]


print('Load Models')
loadModel({'Name': 'Main', 'Filename':filename})

createPlane({
    'Name' : 'Slice',
    'Origin' : [0,0,0.2],
    'Normal' : [0,0,1],
    'Size' : 5000
});

print('Repair Model')
repairModel({'Name':'Main'})

print('Center Model')

boundary = modelBoundary({'Name':'Main'})
BottomCenter = boundary['Min']
BottomCenter[0] = (BottomCenter[0] + boundary['Max'][0])/2.0
BottomCenter[1] = (BottomCenter[1] + boundary['Max'][1])/2.0

offset = [0,0,0];
offset[0] = BuildplateCenter[0]-BottomCenter[0];
offset[1] = BuildplateCenter[1]-BottomCenter[1];
offset[2] = BuildplateCenter[2]-BottomCenter[2];

transformModel({'Name':'Main','Offset':offset})
transformModel({'Name':'Slice','Offset':offset})

print('Prepare Slice Region')
boundary = modelBoundary({'Name':'Main'})
sliceMin=boundary['Min']
sliceMin[0] = sliceMin[0] - boundary['Size'][0]/10.0;
sliceMin[1] = sliceMin[1] - boundary['Size'][1]/10.0;
sliceMin[2] = sliceMin[2] - boundary['Size'][2]/10.0;

sliceMax=boundary['UniformMax']
sliceMax[0] = sliceMax[0] + boundary['UniformSize'][0]/10.0;
sliceMax[1] = sliceMax[1] + boundary['UniformSize'][1]/10.0;
sliceMax[2] = sliceMax[2] + boundary['UniformSize'][2]/10.0;

print('Slice Y')

sliceModel({
    'ModelName'    : 'Main',
    'ToolName'     : 'Slice',
    'Mode'         : 'Model',
    'ResultName'   : 'SliceY'
});

print('Slice X')
streaks = []
streakCount = 0;

nozzleDiameter = 0.4;

X = sliceMin[0];
counter = 0;
while(X < sliceMax[0]):
  createPlane({
      'Name' : 'SliceX',
      'Origin' : [X,0,0.2],
      'Normal' : [1,0,0],
      'Size' : 5000
  });
  streakName = 'Streak' + str(counter);
  sliceModel({
      'ModelName'  : 'SliceY',
      'ToolName'   : 'SliceX',
      'Mode'       : 'Line',
      'LineName'   : streakName
  });

  streaks.append(streakName);

  X = X + nozzleDiameter;
  counter = counter+1;
  
  
print('Merge Lines');

mergeLines({
    'Result':'Merged',
    'Radius' : 0.5,
    'Input' : streaks
});

print('GCode Generation');

gcode = ""

gcode += setHeat({
    'Mode' : 'Set',
    'Nozzle' : 215,
    'Bed': 60
});

gcode += startup({});
#gcode += prime({});

gcode += "\n; ;;;;;;;;;;;;;;;;;;";
gcode += "\n; Printing:  ";
gcode += "\n; ;;;;;;;;;;;;;;;;;;\n";

gcode += linearPrint({
  'Line': 'Merged',
  'Feedrate': 0.03
})

gcode += shutdown({});

saveText({
    'Text' : gcode,
    'Filename' : 'hello.gcode'
});

print('Finished script')

