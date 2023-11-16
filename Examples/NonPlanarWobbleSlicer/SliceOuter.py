
print('Slice Outer');

config      = getData({'Name':'config'});
NozzleDiameter = config['NozzleDiameter'];

amountErosion = getData({'Name':'OuterAreaWallAmount'});

zCounter = getData({'Name':'ZCounter'});
streaks = [];
for i in range(0,amountErosion):
  print(i);
  streakname = "Streak" + str(i);
  sliceModel({
      'ModelName'    : 'OuterAreaWall' + str(i),
      'ToolName'     : 'Slice',
      'Mode'         : 'Line',
      'LineName'   : streakname 
  });
  streaks.append(streakname);
  #saveText({
  #  'Text' : linearPrint({'Line': streakname,'Feedrate': 0.03}),
  #  'Filename' : "dbg/SliceOuter/layer" + str(zCounter) + "ero" + str(i) + ".gcode"});
  sliceModel({
      'ModelName'    : 'OuterAreaWall' + str(i),
      'ToolName'     : 'Slice',
      'Mode'         : 'Model',
      'ResultName'   : 'dbg'
  });
  #saveModel({"Name":"dbg","Filename":"dbg/SliceOuter/layer" + str(zCounter) + "ero" + str(i) + ".stl"});

mergeLines({
    'Result':'Result' + str(zCounter),
    'Radius' : 0.5,
    'Input' : streaks
});
setData({'Name':'ZCounter', 'Data' : zCounter+1});