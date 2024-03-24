
print("Prepare Config");
config = {
  'Filename'          : '3DBenchy.stl',
  'ResultFilename'    : '3DBenchy.gcode',
  'BuildplateCenter'  : [100,100,0],
  'LayerHeight'       : 0.2,
  'WallAmount'        : 2,
  'NozzleDiameter'    : 0.40,
  'VoxelResolution'   : 128, #dividable by 8
  'PlanarSlicing'     : False,
  'LayerLimit'        : -1, # -1 for unlimited
  'IntermediateFiles' : True,
  # Non Planar Sinus Wobble:
  'WobbleAmplitude'   : 1.0, # z height
  'WobbleFrequency'   : 4.14*15, #amount of waves in full volume
  'WobbleFrom'        : 0.11, # percentage where wobble is full
  'WobbleTo'          : 0.51, #percentage where wobble stops being full
  'rampTime'          : 0.1 #percentage before and after full wobble where it fades out
};
setData({'Name':'config', 'Data' : config });

resolution     = [config["VoxelResolution"],config["VoxelResolution"],config["VoxelResolution"]]
LayerHeight    = config["LayerHeight"];
NozzleDiameter =config["NozzleDiameter"];

###############
# Preparation
###############

print("Load Model");
loadModel({'Name': 'Main', 'Filename': config["Filename"]})
bounds = modelBoundary({
    'Name'  : 'Main',
    'Padding' : 1 # 1 greater in each direction (e.g. 2*VoxelLength)
});

minPos         = bounds["Min"];
maxPos         = bounds["UniformMax"];
cubeSize       = maxPos[0]-minPos[0]
voxelLength    = cubeSize / resolution[0];
print("Voxel Length: " + str(voxelLength));

print("Voxelize Model");
voxelizeModel({
    'ModelName'  : 'Main',
    'VoxelName'  : 'Main',
    'Resolution' : resolution, # divideable by 8
    'Start'      : bounds["Min"],
    'End'        : bounds["UniformMax"]
});
Model = GetFarfalleBool("Main");

print("Init Z Layer");
createVolume({
    'Name': 'ZLayer',
    'Resolution': resolution, #dividable by 8
    'Type' : 'Double' # 'Bool' / 'Double' / 'Int'
});
ZLayer = GetFarfalleDouble("ZLayer");
for x in range(resolution[0]):
    for y in range(resolution[1]):
        for z in range(resolution[2]):
                ZLayer[x,y,z] = z * voxelLength;

print("Init X Layer");
createVolume({
    'Name': 'XLayer',
    'Resolution': resolution, #dividable by 8
    'Type' : 'Double' # 'Bool' / 'Double' / 'Int'
});
XLayer = GetFarfalleDouble("XLayer");
for x in range(resolution[0]):
    for y in range(resolution[1]):
        for z in range(resolution[2]):
                XLayer[x,y,z] = x * voxelLength;

print("Init Canvas");
createVolume({
    'Name': 'Canvas',
    'Resolution': resolution, # dividable by 8
    'Type' : 'Bool' # 'Bool' / 'Double' / 'Int'
});
Canvas = GetFarfalleBool("Canvas");

print("Create First Line");
import time
start_time = time.time()

layerPos = bounds["Min"][2] + voxelLength*20;
XPos = bounds["Min"][0] + NozzleDiameter*80;
for x in range(resolution[0]):
    for y in range(resolution[1]):
        for z in range(resolution[2]):
                InZLayer = ZLayer[x,y,z] < layerPos + LayerHeight and ZLayer[x,y,z] > layerPos - LayerHeight;
                InXLayer = XLayer[x,y,z] < XPos + voxelLength and XLayer[x,y,z] > XPos - voxelLength;
                Canvas[x,y,z] = InXLayer and InZLayer and Model[x,y,z];
print("--- %s seconds ---" % (time.time() - start_time))

print("Triangulate");
triangulateVolume({
    'VoxelName': 'Canvas',
    'ModelName': 'Canvas',
    'Start' : bounds["Min"],
    'End'   : bounds["UniformMax"]
});

print("Save");

saveModel({
    'Name' : 'Canvas',
    'Filename': 'Canvas.stl'
});

print("Finished");