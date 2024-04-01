print("Hello World");



loadModel({
    'Name' : 'Benchy',
    'Filename': '3DBenchy.stl'
});


bounds = modelBoundary({
    'Name'  : 'Benchy',
    'Padding' : 1 # 1 greater in each direction (e.g. 2*VoxelLength)
});

Start    = bounds["Min"]
End      = bounds["Max"];
Start[2] = Start[2] + 3;
End[2]   = Start[2] + 1;

print("Voxelize");

voxelizeModel({
    'ModelName'  : 'Benchy',
    'VoxelName'  : 'Benchy',
    'Resolution' : [128,128,8], # divideable by 8
    'Start'      : Start,
    'End'        : End
});
 
transformDistanceMap({
  'DistanceMapName' : 'Main',
  'VoxelName'   : 'Main',
  'Distance' : 0.5*(NozzleDiameter/voxelSize)
});
  
print("Save Vox");

saveMagicaVox({
    'VoxelField'     : 'Benchy',
    'Filename'       : 'Benchy.vox'
});

print("Triangulate")

triangulateVolume({
    'VoxelName': 'Benchy',
    'ModelName': 'BenchyRemeshed',
    'Start' : Start,
    'End'   : End
});

print("Save");

saveModel({
    'Name' : 'BenchyRemeshed',
    'Filename': 'BenchyRemeshed.stl'
});

print("Finished");