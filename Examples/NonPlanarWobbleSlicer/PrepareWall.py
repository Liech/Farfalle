print('Prepare Wall')

config          = getData({'Name':'config'});
VoxelResolution = config['VoxelResolution'];
WallThickness   = config['WallThickness'];
NozzleDiameter  = config['NozzleDiameter'];
Detail  = config['Detail'];

# for packed variant
boundary = voxelizationBoundary({'Name':'Main', 'Resolution':VoxelResolution[0], 'OuterVoxels' : 2})
voxelizeMin=boundary['Min']
voxelizeMax=boundary['Max']

distanceMap({
    'VoxelName'   : 'OuterArea',
    'DistanceMapName' : 'OuterArea',
    'Resolution' : VoxelResolution,
    'Mode' : 'XY' # XY / XYZ
});


voxelSize = (voxelizeMax[0]-voxelizeMin[0]) / VoxelResolution[0];

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
  
  #print('Packing')
  #packVolume({
  #  'VoxelName':'Erosion',
  #  'DoubleName':'Main',
  #  'Resolution':VoxelResolution,
  #  'PackageSize':[2,2,2]
  #});
  
  print('Triangulation')
  
  name= 'OuterAreaWall' + str(counter)
  #triangulateDouble({
  #  'DoubleName':'Main',
  #  'ModelName' :name,
  #  'Resolution':[VoxelResolution[0]/2,VoxelResolution[1]/2,VoxelResolution[1]/2],
  #  'Start'     :voxelizeMin,
  #  'End'       :voxelizeMax
  #});
  triangulateVolume({
    'VoxelName': 'Erosion',
    'ModelName': name,
    'Resolution' : VoxelResolution,
    'Start' : voxelizeMin,
    'End'   : voxelizeMax
  });
  if (Detail < 1):
    simplifyModel({'Name':name, "Ratio":Detail})
    
  #saveModel({"Name":name,"Filename":name + ".stl"});
  
  hasTri = hasAnyTriangle({'Name':name});
  if (not hasTri):
    break;
    
  eroded+=NozzleDiameter/2
  counter = counter+1;

setData({'Name':'OuterAreaWallAmount', 'Data':counter});