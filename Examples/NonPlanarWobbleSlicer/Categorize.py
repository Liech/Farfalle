
config          = getData({'Name':'config'});
VoxelResolution = config['VoxelResolution'];
WallThickness   = config['WallThickness'];
NozzleDiameter   = config['NozzleDiameter'];


print('Voxelize Model')
boundary = modelBoundary({'Name':'Main'})
voxelizeMin=boundary['Min']
voxelizeMin[0] = voxelizeMin[0] - boundary['Size'][0]/10.0;
voxelizeMin[1] = voxelizeMin[1] - boundary['Size'][1]/10.0;
voxelizeMin[2] = voxelizeMin[2] - boundary['Size'][2]/10.0;

voxelizeMax=boundary['UniformMax']
voxelizeMax[0] = voxelizeMax[0] + boundary['UniformSize'][0]/10.0;
voxelizeMax[1] = voxelizeMax[1] + boundary['UniformSize'][1]/10.0;
voxelizeMax[2] = voxelizeMax[2] + boundary['UniformSize'][2]/10.0;

voxelizeModel({
  'ModelName' : 'Main',
  'VoxelName' : 'Main',
  'Resolution': VoxelResolution,
  'Start'     : voxelizeMin,
  'End'       : voxelizeMax
});

print('Distance Map')

distanceMap({
    'VoxelName'   : 'Main',
    'DistanceMapName' : 'Main',
    'Resolution' : VoxelResolution,
    'Mode' : 'XY' # XY / XYZ
});

voxelSize = boundary["UniformSize"][0] / VoxelResolution[0];
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
simplifyModel({'Name':'InnerArea', "Ratio":0.3})
simplifyModel({'Name':'OuterArea', "Ratio":0.3})
saveModel({"Name":"InnerArea","Filename":"slice/InnerArea.stl"});
saveModel({"Name":"OuterArea","Filename":"slice/OuterArea.stl"});