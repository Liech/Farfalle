
loadModel({
    'Name' : 'Benchy',
    'Filename': '3DBenchy.stl'
});

bounds = modelBoundary({
    'Name'  : 'Benchy',
    'Padding' : 5 # 2 greater in each direction (e.g. 2*VoxelLength)
});

print(bounds);

voxelizeModel({
    'ModelName'  : 'Benchy',
    'VoxelName'  : 'Benchy',
    'Resolution' : [128,128,128], # divideable by 8
    'Start'      : bounds["Min"],
    'End'        : bounds["UniformMax"]
});

triangulateVolume({
    'VoxelName': 'Benchy',
    'ModelName': 'BenchyRemeshed',
    'Start' : bounds["Min"],
    'End'   : bounds["UniformMax"]
});

saveModel({
    'Name' : 'BenchyRemeshed',
    'Filename': 'BenchyRemeshed.stl'
});