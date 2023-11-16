print('Categorize')

config          = getData({'Name':'config'});
VoxelResolution = config['VoxelResolution'];
WallThickness   = config['WallThickness'];
NozzleDiameter   = config['NozzleDiameter'];


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
print('  Remesh Model')
triangulateVolume({
    'VoxelName': 'Main',
    'ModelName': 'Remeshed',
    'Resolution' : VoxelResolution,
    'Start' : voxelizeMin,
    'End'   : voxelizeMax
}); 
print('  Save Model')
#saveModel({"Name":"Remeshed","Filename":"remeshed.stl"});


print('Distance Map')

distanceMap({
    'VoxelName'   : 'Main',
    'DistanceMapName' : 'Main',
    'Resolution' : VoxelResolution,
    'Mode' : 'XY' # XY / XYZ
});



print('  transform for remesh');
transformDistanceMap({
    'DistanceMapName' : 'Main',
    'VoxelName'   : 'Remeshed',
    'Distance' : 1e-7
});
print('  remesh distancemap');
triangulateVolume({
    'VoxelName': 'Main',
    'ModelName': 'Remeshed',
    'Resolution' : VoxelResolution,
    'Start' : voxelizeMin,
    'End'   : voxelizeMax
}); 
#saveModel({"Name":"Remeshed","Filename":"dist_remeshed.stl"});


voxelSize = (voxelizeMax[0]-voxelizeMin[0]) / VoxelResolution[0];
print('Voxel Size: ' + str(voxelSize))

eroded = 0.0;

print('Mapping Areas');

transformDistanceMap({
    'DistanceMapName' : 'Main',
    'VoxelName'   : 'InnerArea',
    'Distance' : (NozzleDiameter*2/voxelSize)
});
copyVoxel({
    'Source' : 'Main',
    'Target'   : 'OuterArea'
});
subtractVoxel({
    'Target' : 'OuterArea',
    'Tool'   : 'InnerArea'
});
triangulateVolume({
    'VoxelName': 'OuterArea',
    'ModelName': 'OuterArea',
    'Resolution' : VoxelResolution,
    'Start' : voxelizeMin,
    'End'   : voxelizeMax
}); 
triangulateVolume({
    'VoxelName': 'InnerArea',
    'ModelName': 'InnerArea',
    'Resolution' : VoxelResolution,
    'Start' : voxelizeMin,
    'End'   : voxelizeMax
}); 
#simplifyModel({'Name':'InnerArea', "Ratio":0.3})
#simplifyModel({'Name':'OuterArea', "Ratio":0.3})
#saveModel({"Name":"InnerArea","Filename":"dbg/InnerArea.stl"});
#saveModel({"Name":"OuterArea","Filename":"dbg/OuterArea.stl"});