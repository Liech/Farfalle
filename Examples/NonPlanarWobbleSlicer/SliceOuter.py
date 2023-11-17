
print('Slice Outer');

config          = getData({'Name':'config'});
NozzleDiameter  = config['NozzleDiameter'];
IO              = config['IntermediateFiles'];

wallAmount = getData({'Name':'OuterAreaWallAmount'});

zCounter = getData({'Name':'ZCounter'});
streaks = [];
for i in range(0,wallAmount):
  print(i);
  streakname = "Streak" + str(i);
  sliceModel({
      'ModelName'    : 'OuterAreaWall' + str(i),
      'ToolName'     : 'Slice',
      'Mode'         : 'Line',
      'LineName'   : streakname 
  });
  streaks.append(streakname);

  if (IO):
    saveText({
      'Text' : linearPrint({'Line': streakname,'Feedrate': 0.03}),
      'Filename' : "dbg/slice/layer" + str(zCounter) + "_wall" + str(i) + ".gcode"});
    sliceModel({
      'ModelName'    : 'OuterAreaWall' + str(i),
      'ToolName'     : 'Slice',
      'Mode'         : 'Model',
      'ResultName'   : 'dbg'
    });
    saveModel({"Name":"dbg","Filename":"dbg/slice/layer" + str(zCounter) + "_wall" + str(i) + ".stl"});

mergeLines({
    'Result':'Result' + str(zCounter),
    'Radius' : 0.5,
    'Input' : streaks
});
setData({'Name':'ZCounter', 'Data' : zCounter+1});