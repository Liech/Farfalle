
loadModel({
    'Name' : 'Benchy',
    'Filename': '3DBenchy.stl',
    'Mode' : 'SOUP' # CGAL / SOUP
});

bounds = modelBoundary({
    'Name'  : 'Benchy',
    'Mode'  : 'SOUP', # 'CGAL' / 'SOUP'
    'Padding' : 5 # 2 greater in each direction (e.g. 2*VoxelLength)
});

print(bounds);
print("Voxelize");

voxelizeModel({
    'ModelName'  : 'Benchy',
    'VoxelName'  : 'Benchy',
    'Resolution' : [128,128,128], # divideable by 8
    'Start'      : bounds["Min"],
    'End'        : bounds["UniformMax"],
    'Mode'       : "SOUP" # "CGAL" / "SOUP"
});

print("Triangulate");

triangulateVolume({
    'VoxelName': 'Benchy',
    'ModelName': 'BenchyRemeshed',
    'Start' : bounds["Min"],
    'End'   : bounds["UniformMax"],
    'Mode' : 'SOUP' # 'CGAL' / 'SOUP'
});

print("Save");

saveModel({
    'Name' : 'BenchyRemeshed',
    'Filename': 'BenchyRemeshed.stl',
    'Mode' : 'SOUP' # 'CGAL' / 'SOUP'
});

print("Finished");