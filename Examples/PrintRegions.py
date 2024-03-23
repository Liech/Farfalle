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

resolution = [512,512,512]
voxelizeModel({
  'ModelName' : 'Main',
  'VoxelName' : 'Main',
  'Resolution': resolution,
  'Start'     : voxelizeMin,
  'End'       : voxelizeMax
});



print('Distance Map')

distanceMap({
    'VoxelName'   : 'Main',
    'DistanceMapName' : 'Main',
    'Resolution' : resolution,
    'Mode' : 'XY' # XY / XYZ
});

voxelSize = boundary["UniformSize"][0] / resolution[0];
print('Voxel Size: ' + str(voxelSize))

nozzlediam = 0.4
eroded = 0.0;

print('Mapping Areas');

transformDistanceMap({
    'DistanceMapName' : 'Main',
    'VoxelName'   : 'InnerArea',
    'Distance' : (nozzlediam*2/voxelSize)
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
    'Resolution' : resolution,
    'Start' : voxelizeMin,
    'End'   : voxelizeMax
}); 
triangulateVolume({
    'VoxelName': 'InnerArea',
    'ModelName': 'InnerArea',
    'Resolution' : resolution,
    'Start' : voxelizeMin,
    'End'   : voxelizeMax
}); 
saveModel({
    'Name' : 'OuterArea',
    'Filename': 'OuterArea.stl'
});
saveModel({
    'Name' : 'InnerArea',
    'Filename': 'InnerArea.stl'
});