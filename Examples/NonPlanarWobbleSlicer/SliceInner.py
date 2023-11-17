
print('Slice Inner');

config      = getData({'Name':'config'});
NozzleDiameter = config['NozzleDiameter'];
IO              = config['IntermediateFiles'];


zCounter = getData({'Name':'ZCounter'});
#saveModel({"Name":"Slice","Filename":"slice/ToolSlice" + str(zCounter) + ".stl"});
#saveModel({"Name":"InnerArea","Filename":"slice/SliceInner" + str(zCounter) + ".stl"});

print("  slicing");
sliceModel({
    'ModelName'    : 'InnerArea',
    'ToolName'     : 'Slice',
    'Mode'         : 'Model',
    'ResultName'   : 'SliceZ'
});
print("  sliced");
if (IO):
  saveModel({"Name":"SliceZ","Filename":"dbg/SliceZ" + str(zCounter) + ".stl"});

boundary = modelBoundary({'Name':'SliceZ'})
sliceMin=boundary['Min']
sliceMin[0] = sliceMin[0] - boundary['Size'][0]/10.0;
sliceMin[1] = sliceMin[1] - boundary['Size'][1]/10.0;
sliceMin[2] = sliceMin[2] - boundary['Size'][2]/10.0;

sliceMax=boundary['Max']
sliceMax[0] = sliceMax[0] + boundary['Size'][0]/10.0;
sliceMax[1] = sliceMax[1] + boundary['Size'][1]/10.0;
sliceMax[2] = sliceMax[2] + boundary['Size'][2]/10.0;

streaks = []
streakCount = 0;

X = sliceMin[0];
counter = 0;
while(X < sliceMax[0]):
  createPlane({
      'Name' : 'SliceX',
      'Origin' : [X,0,0.2],
      'Normal' : [1,0,0],
      'Size' : 5000
  });
  if (IO):
    saveModel({"Name":"SliceX","Filename":"dbg/SliceX" + str(X) + ".stl"});
  streakName = 'Streak' + str(counter);
  sliceModel({
      'ModelName'  : 'SliceZ',
      'ToolName'   : 'SliceX',
      'Mode'       : 'Line',
      'LineName'   : streakName
  });
  if (IO):
    saveText({
      'Text' : linearPrint({'Line': streakName,'Feedrate': 0.03}),
      'Filename' : "dbg/slice/layer" + str(zCounter) + "_inner" + str(X) + ".gcode"});
  streaks.append(streakName);

  X = X + NozzleDiameter;
  counter = counter+1;
  

zCounter = getData({'Name':'ZCounter'});
mergeLines({
    'Result':'Result' + str(zCounter),
    'Radius' : 0.5,
    'Input' : streaks
});
setData({'Name':'ZCounter', 'Data' : zCounter+1});
