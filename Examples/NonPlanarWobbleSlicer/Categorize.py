print('Categorize')

config          = getData({'Name':'config'});
VoxelResolution = config['VoxelResolution'];
WallThickness   = config['WallThickness'];
NozzleDiameter  = config['NozzleDiameter'];
Detail          = config['Detail'];
WallAmount      = config['WallAmount'];
IO              = config['IntermediateFiles'];

print('Voxelize Model')
boundary = voxelizationBoundary({'Name':'Main', 'Resolution':VoxelResolution[0], 'OuterVoxels' : 2})
voxelizeMin=boundary['Min']
voxelizeMax=boundary['Max']
print('  ' + str(boundary))

print('  Voxelize Model')
voxelizeModel({
  'ModelName' : 'Main',
  'VoxelName' : 'Main',
  'Resolution': VoxelResolution,
  'Start'     : voxelizeMin,
  'End'       : voxelizeMax
});
if (IO):
  print('  Remesh Model')
  triangulateVolume({
      'VoxelName': 'Main',
      'ModelName': 'Remeshed',
      'Resolution' : VoxelResolution,
      'Start' : voxelizeMin,
      'End'   : voxelizeMax
  }); 
  print('  Save Model')
  saveModel({"Name":"Remeshed","Filename":"dbg/remeshed.stl"});


print('Distance Map')

distanceMap({
    'VoxelName'   : 'Main',
    'DistanceMapName' : 'Main',
    'Resolution' : VoxelResolution,
    'Mode' : 'XY' # XY / XYZ
});

voxelSize = (voxelizeMax[0]-voxelizeMin[0]) / VoxelResolution[0];
print('Voxel Size: ' + str(voxelSize))

eroded = 0.0;

print('Mapping Areas');



for i in range(0,WallAmount+1):
  print('Wall Number: ' + str(i) + "/" + str(WallAmount));
  name= 'OuterAreaWall' + str(i)
  transformDistanceMap({
    'DistanceMapName' : 'Main',
    'VoxelName'   : 'Erosion',
    'Distance' : (i+0.5)*(NozzleDiameter/voxelSize)
  });
  if (i==WallAmount):
    name = "InnerArea";  
  triangulateVolume({
    'VoxelName': 'Erosion',
    'ModelName': name,
    'Resolution' : VoxelResolution,
    'Start' : voxelizeMin,
    'End'   : voxelizeMax
  });
  if (IO):
    saveModel({"Name":name,"Filename":"dbg/" + name + ".stl"});
  if (Detail < 1):
    simplifyModel({'Name':name, "Ratio":Detail})
    if (IO):
      saveModel({"Name":name,"Filename":"dbg/" + name + "_simplified.stl"});
    
setData({'Name':'OuterAreaWallAmount', 'Data':WallAmount});