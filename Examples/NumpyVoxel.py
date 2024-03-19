
import numpy

print("Hello World");
res =  [8,8,8]
createVolume({"Name":"Kartoffel", "Resolution":res});
data = GetFarfalleVoxel("Kartoffel");
data[1,1,3] = True
data[4,4,3] = True
data[4,5,3] = True
data[6,6,3] = True

triangulateVolume({
    'VoxelName': 'Kartoffel',
    'ModelName': 'Kartoffel',
    'Resolution' : res,
    'Start' : [0,0,0],
    'End'   : [5,5,5]
});
saveModel({
    'Name' : 'Kartoffel',
    'Filename': 'Kartoffel.stl'
});