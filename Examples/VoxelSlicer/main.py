
print("Prepare Config");
config = {
  'Filename'          : '3DBenchy.stl',
  'ResultFilename'    : '3DBenchy.gcode',
  'BuildplateCenter'  : [100,100,0],
  'LayerHeight'       : 0.2,
  'WallAmount'        : 2,
  'NozzleDiameter'    : 0.40,
  'VoxelResolution'   :[128,128,128], #dividable by 8
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

print("Load Model");
loadModel({'Name': 'Main', 'Filename': config["Filename"]})

print("Init Empty Z Layer");
createVolume({
    'Name': 'ZLayer',
    'Resolution': config["VoxelResolution"], #dividable by 8
    'Type' : 'Double' # 'Bool' / 'Double' / 'Int'
});
ZLayer = GetFarfalleDouble("ZLayer");

print("Init Empty X Layer");
createVolume({
    'Name': 'XLayer',
    'Resolution': config["VoxelResolution"], #dividable by 8
    'Type' : 'Double' # 'Bool' / 'Double' / 'Int'
});
XLayer = GetFarfalleDouble("XLayer");


print("Finished");