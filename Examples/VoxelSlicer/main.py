
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
NozzleDiameter = config["NozzleDiameter"];

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

ZIso = bounds["Min"][2] + voxelLength*20;
XIso = bounds["Min"][0] + NozzleDiameter*110;
for x in range(resolution[0]-1):
    for y in range(resolution[1]-1):
        for z in range(resolution[2]-1):
                z0 = ZLayer[x,y,z] < ZIso
                z1 = ZLayer[x,y,z+1] < ZIso
                x0 = XLayer[x,y,z] < XIso
                x1 = XLayer[x+1,y,z] < XIso
                isZIsosurface = (z1 and not z0) or (not z1 and z0)
                isXIsosurface = (x1 and not x0) or (not x1 and x0)
                isTrue = Model[x,y,z] and isZIsosurface and isXIsosurface;
                Canvas[x,y,z] = isTrue

print("--- %s seconds ---" % (time.time() - start_time))

print("Trace Voxels");
traceVoxelLines({
    'Source' : 'Canvas',
    'Target' : 'LineCollection',
    'Min'    : bounds["Min"],
    'Max'    : bounds["UniformMax"]
});

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


print('Write GCode');
ResultFilename = config['ResultFilename'];

gcode = ""
gcode += setHeat({
    'Mode' : 'Set',
    'Nozzle' : 215,
    'Bed': 60
});

gcode += startup({});
#gcode += prime({});
gcode += "\n; ;;;;;;;;;;;;;;;;;;";
gcode += "\n; Printing:  ";
gcode += "\n; ;;;;;;;;;;;;;;;;;;\n";

gcode += linearPrint({
  'Line': "LineCollection",
  'Feedrate': 0.028
})

gcode += shutdown({});

saveText({
    'Text' : gcode,
    'Filename' : ResultFilename
});

print("Finished");