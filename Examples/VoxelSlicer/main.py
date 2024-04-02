import numpy
import time

print("Prepare Config");
config = {
  'Filename'          : '3DBenchy.stl',
  'ResultFilename'    : '3DBenchy.gcode',
  'BuildplateCenter'  : [100,100,0],
  'LayerHeight'       : 0.2,
  'WallAmount'        : 2,
  'NozzleDiameter'    : 0.40,
  'VoxelResolution'   : 256, #dividable by 8
  'LayerLimit'        : -1, # -1 for unlimited
  'WindowSize'        : 1 #mm   -- sliding voxel window
};
setData({'Name':'config', 'Data' : config });

resolution     = [config["VoxelResolution"],config["VoxelResolution"],config["VoxelResolution"]]
LayerHeight    = config["LayerHeight"];
NozzleDiameter = config["NozzleDiameter"];

###############
# Preparation
###############
padding = 1;

print("Load Model");
loadModel({'Name': 'Main', 'Filename': config["Filename"]})
bounds = modelBoundary({
    'Name'  : 'Main',
    'Padding' : 1 # 1 greater in each direction (e.g. 2*VoxelLength)
});

minPos         = bounds["Min"];
maxPos         = bounds["Max"];
cubeSize       = maxPos[0]-minPos[0]
voxelLength    = [
(maxPos[0]-minPos[0]) / resolution[0],
(maxPos[1]-minPos[1]) / resolution[1],
(maxPos[2]-minPos[2]) / resolution[2],
];
print("Voxel Length: " + str(voxelLength));

windowSize = config["WindowSize"]
ZResolution = int(windowSize / LayerHeight);
ZResolution = ZResolution + (8-ZResolution%8); # has to be %8==0
print("Z Window Resolution: " + str(ZResolution));

print("Voxelize");
voxelizeModel({
  'ModelName'  : 'Main',
  'VoxelName'  : 'Main',
  'Resolution' : [resolution[0],resolution[1],resolution[2]], # divideable by 8
  'Start'      : bounds["Min"],
  'End'        : bounds["Max"]
});
saveMagicaVox({
    'VoxelField'     : 'Main',
    'Filename'       : "dbg/Main.vox"
});

print("Distance Map");

distanceMap({
    'VoxelName'   : 'Main',
    'DistanceMapName' : 'DistanceMap',
    'Resolution' : resolution,
    'Mode' : 'XY' # XY / XYZ
});
distanceMapValue = GetFarfalleInt("DistanceMap");



int2double({
    'IntField'     : 'DistanceMap',
    'DoubleField'  : 'DistanceMap',
    'Scale'      : NozzleDiameter/voxelLength[0]
});

saveMagicaVox({
    'VoxelField'     : 'Main',
    'Filename'       : "dbg/Main_Shaved.vox"
});

# Shave of walls
transformDistanceMap({
  'DistanceMapName' : 'DistanceMap',
  'VoxelName'   : 'Inner',
  'Distance' : (0.5 + config["WallAmount"])*(NozzleDiameter/voxelLength[0])
});

#print("Debug Step");
#createVolume({
#    'Name': 'Debug',
#    'Resolution': [resolution[0],resolution[1],resolution[2]], #dividable by 8
#    'Type' : 'Bool' # 'Bool' / 'Double' / 'Int'
#});
#Debug = GetFarfalleBool("Debug");
#for x in range(resolution[0]):
#  for y in range(resolution[1]):
#      for z in range(resolution[0]):
#              Debug[x,y,z] = distanceMapValue[x,y,z] == 1;
#saveMagicaVox({
#    'VoxelField'     : 'Debug',
#    'Filename'       : "dbg/Debug.vox"
#});

print("Init Z Layer");
createVolume({
    'Name': 'ZLayer',
    'Resolution': [resolution[0],resolution[1],resolution[2]], #dividable by 8
    'Type' : 'Double' # 'Bool' / 'Double' / 'Int'
});
ZLayer = GetFarfalleDouble("ZLayer");

print("Fill Z Layer");
for x in range(resolution[0]):
  for y in range(resolution[1]):
      for z in range(resolution[0]):
              ZLayer[x,y,z] = bounds["Min"][2] + z * voxelLength[2];
                
print("Init X Layer");
createVolume({
    'Name': 'XLayer',
    'Resolution': [resolution[0],resolution[1],resolution[2]], #dividable by 8
    'Type' : 'Double' # 'Bool' / 'Double' / 'Int'
});
XLayer = GetFarfalleDouble("XLayer");

print("Fill X Layer");
for x in range(resolution[0]):
  for y in range(resolution[1]):
      for z in range(resolution[0]):
              XLayer[x,y,z] = bounds["Min"][0] + x * voxelLength[0];

print("Init Canvas");
createVolume({
    'Name': 'Canvas',
    'Resolution': [resolution[0],resolution[1],ZResolution], # dividable by 8
    'Type' : 'Bool' # 'Bool' / 'Double' / 'Int'
});
Canvas = GetFarfalleBool("Canvas");

print("Work");

counter = 0;
startZ = minPos[2] + padding - voxelLength[2];
endZ   = maxPos[2] - padding + voxelLength[2];
startTime = time.time()
endTime   = time.time()

maxCounter = int((endZ-startZ) / LayerHeight)

###############
# Execution
###############

for windowPos in numpy.arange(startZ,endZ,LayerHeight):
  
  completion = (windowPos-startZ)/(endZ-startZ);
  endTime    = time.time()
  etastr = "..."
  if (counter > 5):
    eta        = ((endTime-startTime) / counter) * (maxCounter-counter)
    etastr = "{:d}m {:d}s      ".format(int(eta/60),int(eta%60));
  
  print("Progress: {:.2f}%".format(100*completion) + "  -  " + etastr, end="\r");
  counter = counter + 1

  windowMin = bounds["Min"].copy() 
  windowMax = bounds["Max"].copy() 
  windowMin[2] = windowPos - windowSize/2.0;
  windowMax[2] = windowPos + windowSize/2.0; 
   
  windowMinVox = [0,0,int((windowMin[2] - bounds["Min"][2]) / voxelLength[2])];
  windowMaxVox = [resolution[0],resolution[1],windowMinVox[2] + ZResolution];
  windowMinVox[2] = max(windowMinVox[2],0)
  windowMaxVox[2] = min(windowMaxVox[2],resolution[2]);
  ZIso = windowPos;
  
  #for XIso in numpy.arange(bounds["Min"][0],bounds["Max"][0],NozzleDiameter):
  #  #print(str(XIso) + " - "  + str(ZIso));
  #  
  #  inputDualIso = {
  #      'VoxelField'     : 'Inner',
  #      'ResultField'    : 'Canvas',
  #      'DensityField1'  : 'XLayer',
  #      'DensityField2'  : 'ZLayer',
  #      'Isovalue1'      : XIso,
  #      'Isovalue2'      : ZIso,
  #      'StartVoxel'     : windowMinVox, # optional
  #      'EndVoxel'       : windowMaxVox, # optional
  #  }
  #  #print(inputDualIso)
  #  dualIsoVoxel(inputDualIso);
  #  traceVoxelLines({
  #      'Source' : 'Canvas',
  #      'Target' : "LineCollection",# + str(counter),
  #      'Min'    : windowMin,
  #      'Max'    : windowMax
  #  });
  #  #saveMagicaVox({
  #  #'VoxelField'     : 'Canvas',
  #  #'Filename'       : "dbg/" + str(counter) + "_" + str(XIso) + ".vox"
  #  #});

  for wall in range(0,config["WallAmount"]):
    dualIsoVoxel({
        'VoxelField'     : 'Main',
        'ResultField'    : 'Canvas',
        'DensityField1'  : 'DistanceMap',
        'DensityField2'  : 'ZLayer',
        'Isovalue1'      : (0.5 + wall),
        'Isovalue2'      : ZIso,
        'StartVoxel'     : windowMinVox, # optional
        'EndVoxel'       : windowMaxVox, # optional
    });
    traceVoxelLines({
        'Source' : 'Canvas',
        'Target' : "LineCollection",# + str(counter),
        'Min'    : windowMin,
        'Max'    : windowMax
    });  
    saveMagicaVox({
    'VoxelField'     : 'Canvas',
    'Filename'       : "dbg/wall" + str(counter) + "_" + str(wall) + ".vox"
    });

print("Progress: 100%                          ");

print('Write GCode');
ResultFilename = config['ResultFilename'];

gcode = ""
gcode += setHeat({
    'Mode' : 'Set',
    'Nozzle' : 215,
    'Bed': 60
});

gcode += startup({});
gcode += prime({});
gcode += "\n; ;;;;;;;;;;;;;;;;;;";
gcode += "\n; Printing:  ";
gcode += "\n; ;;;;;;;;;;;;;;;;;;\n";

gcodeline = linearPrint({
  'Line': "LineCollection",# + str(counter),
  'Feedrate': 0.028
})
#print(gcodeline)
gcode += gcodeline;

gcode += shutdown({});

saveText({
    'Text' : gcode,
    'Filename' : config['ResultFilename']
});
  
print("Progress: 100%                                        ");
print("Finished");