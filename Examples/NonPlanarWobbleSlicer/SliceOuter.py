
print('Slice Outer');

config      = getData({'Name':'config'});
NozzleDiameter = config['NozzleDiameter'];
VoxelResolution = config['VoxelResolution'];

amountErosion = getData({'Name':'OuterAreaWallAmount'});


boundary = modelBoundary({'Name':'OuterArea'})
voxelizeMin=boundary['Min']
voxelizeMin[0] = voxelizeMin[0] - boundary['Size'][0]/10.0;
voxelizeMin[1] = voxelizeMin[1] - boundary['Size'][1]/10.0;
voxelizeMin[2] = voxelizeMin[2] - boundary['Size'][2]/10.0;

voxelizeMax=boundary['UniformMax']
voxelizeMax[0] = voxelizeMax[0] + boundary['UniformSize'][0]/10.0;
voxelizeMax[1] = voxelizeMax[1] + boundary['UniformSize'][1]/10.0;
voxelizeMax[2] = voxelizeMax[2] + boundary['UniformSize'][2]/10.0;

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
  saveText({
    'Text' : linearPrint({'Line': streakname,'Feedrate': 0.03}),
    'Filename' : "dbg/SliceOuter/layer" + str(zCounter) + "ero" + str(i) + ".gcode"});
  sliceModel({
      'ModelName'    : 'OuterAreaWall' + str(i),
      'ToolName'     : 'Slice',
      'Mode'         : 'Model',
      'ResultName'   : 'dbg'
  });
  saveModel({"Name":"dbg","Filename":"dbg/SliceOuter/layer" + str(zCounter) + "ero" + str(i) + ".stl"});

mergeLines({
    'Result':'Result' + str(zCounter),
    'Radius' : 0.5,
    'Input' : streaks
});
setData({'Name':'ZCounter', 'Data' : zCounter+1});