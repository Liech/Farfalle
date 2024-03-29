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
Start[2] = Start[2] + 8;
End[2]   = Start[2] + 1;

print("Voxelize");

voxelizeModel({
    'ModelName'  : 'Benchy',
    'VoxelName'  : 'Benchy',
    'Resolution' : [64,64,8], # divideable by 8
    'Start'      : Start,
    'End'        : End
});

print("Save XRAW");

saveXRaw({
    'VoxelField'     : 'Benchy',
    'Filename'       : 'Benchy.xraw'
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