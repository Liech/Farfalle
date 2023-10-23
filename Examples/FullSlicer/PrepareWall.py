print('Prepare Wall')

config          = getData({'Name':'config'});
VoxelResolution = config['VoxelResolution'];
WallThickness   = config['WallThickness'];
NozzleDiameter  = config['NozzleDiameter'];

boundary = modelBoundary({'Name':'Main'})
voxelizeMin=boundary['Min']
voxelizeMin[0] = voxelizeMin[0] - boundary['Size'][0]/10.0;
voxelizeMin[1] = voxelizeMin[1] - boundary['Size'][1]/10.0;
voxelizeMin[2] = voxelizeMin[2] - boundary['Size'][2]/10.0;

voxelizeMax=boundary['UniformMax']
voxelizeMax[0] = voxelizeMax[0] + boundary['UniformSize'][0]/10.0;
voxelizeMax[1] = voxelizeMax[1] + boundary['UniformSize'][1]/10.0;
voxelizeMax[2] = voxelizeMax[2] + boundary['UniformSize'][2]/10.0;

distanceMap({
    'VoxelName'   : 'OuterArea',
    'DistanceMapName' : 'OuterArea',
    'Resolution' : VoxelResolution,
    'Mode' : 'XY' # XY / XYZ
});


voxelSize = boundary["UniformSize"][0] / VoxelResolution[0];

eroded = NozzleDiameter/2;

streaks = []
streakCount = 0;
counter = 0;

while(True):
  distance = (eroded/voxelSize)
  print('  Erosion: ' + str(distance));
  transformDistanceMap({
      'DistanceMapName' : 'OuterArea',
      'VoxelName'   : 'Erosion',
      'Distance' : distance
  });
  
  print('Packing')
  packVolume({
    'VoxelName':'Erosion',
    'DoubleName':'Main',
    'Resolution':VoxelResolution,
    'PackageSize':[2,2,2]
  });
  
  print('Triangulation')
  
  name= 'OuterAreaWall' + str(counter)
  triangulateDouble({
    'DoubleName':'Main',
    'ModelName' :name,
    'Resolution':[VoxelResolution[0]/2,VoxelResolution[1]/2,VoxelResolution[1]/2],
    'Start'     :voxelizeMin,
    'End'       :voxelizeMax
  });
  
  hasTri = hasAnyTriangle({'Name':name});
  if (not hasTri):
    break;
    
  eroded+=NozzleDiameter
  counter = counter+1;

setData({'Name':'OuterAreaWallAmount', 'Data':counter});