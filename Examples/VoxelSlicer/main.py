import numpy

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

print("Init Z Layer");
createVolume({
    'Name': 'ZLayer',
    'Resolution': [resolution[0],resolution[1],ZResolution], #dividable by 8
    'Type' : 'Double' # 'Bool' / 'Double' / 'Int'
});
ZLayer = GetFarfalleDouble("ZLayer");

print("Init X Layer");
createVolume({
    'Name': 'XLayer',
    'Resolution': [resolution[0],resolution[1],ZResolution], #dividable by 8
    'Type' : 'Double' # 'Bool' / 'Double' / 'Int'
});
XLayer = GetFarfalleDouble("XLayer");

print("Init Canvas");
createVolume({
    'Name': 'Canvas',
    'Resolution': [resolution[0],resolution[1],ZResolution], # dividable by 8
    'Type' : 'Bool' # 'Bool' / 'Double' / 'Int'
});
Canvas = GetFarfalleBool("Canvas");

print("Create First Line");


print("Z Resolution: " + str(ZResolution));

counter = 0;
startZ = minPos[2] + padding - voxelLength[2];
endZ   = maxPos[2] - padding + voxelLength[2];
for windowPos in numpy.arange(startZ,endZ,LayerHeight):
  counter = counter + 1
  print("Progress: " + str(100*(windowPos-startZ)/(endZ-startZ)) + "%");
  windowMin = bounds["Min"]
  windowMax = bounds["Max"]
  windowMin[2] = windowPos - windowSize/2.0;
  windowMax[2] = windowPos + windowSize/2.0; 
  
  voxelizeModel({
    'ModelName'  : 'Main',
    'VoxelName'  : 'Main',
    'Resolution' : [resolution[0],resolution[1],ZResolution], # divideable by 8
    'Start'      : windowMin,
    'End'        : windowMax
  });
  #triangulateVolume({
  #  'VoxelName': 'Main',
  #  'ModelName': 'MainSave',
  #  'Start' : windowMin,
  #  'End'   : windowMax
  #});
  #saveModel({
  #    'Name' : 'MainSave',
  #    'Filename': "dbg/voxel" + str(counter) + ".stl"
  #});
  #saveMagicaVox({
  #    'VoxelField'     : 'Main',
  #    'Filename'       : "dbg/voxel" + str(counter) + ".vox"
  #});

  for x in range(resolution[0]):
    for y in range(resolution[1]):
        for z in range(ZResolution):
                ZLayer[x,y,z] = windowPos + z * voxelLength[2] - windowSize/2.0;
                
  for x in range(resolution[0]):
    for y in range(resolution[1]):
        for z in range(ZResolution):
                XLayer[x,y,z] = windowMin[0] + x * voxelLength[0];
                
  for XIso in numpy.arange(windowMin[0],windowMax[0],NozzleDiameter):
    ZIso = windowPos;
    #print(str(XIso) + " - "  + str(ZIso));
    
    dualIsoVoxel({
        'VoxelField'     : 'Main',
        'ResultField'    : 'Canvas',
        'DensityField1'  : 'XLayer',
        'DensityField2'  : 'ZLayer',
        'Isovalue1'      : XIso,
        'Isovalue2'      : ZIso
    });
    traceVoxelLines({
        'Source' : 'Canvas',
        'Target' : "LineCollection",# + str(counter),
        'Min'    : bounds["Min"],
        'Max'    : bounds["Max"]
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
      'Filename' : "dbg/voxel" + str(counter) + ".gcode"
  });
    
  print("Finished");