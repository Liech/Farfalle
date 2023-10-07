filename = "C:\\Users\\nicol\\Downloads\\_3DBenchy_-_The_jolly_3D_printing_torture-test_by_CreativeTools_se_763622\\files\\3DBenchyFixed.stl";
slice = "C:\\Users\\nicol\\Documents\\GitHub\\Farfalle\\out\\dbg\\slice0.stl"

BuildplateCenter = [100,100,0]


print('Load Models')
loadModel({'Name': 'Main', 'Filename':filename})
loadModel({'Name': 'Slice', 'Filename':slice})

print('Repair Model')
repairModel({'Name':'Main'})

print('Center Model')

boundary = modelBoundary({'Name':'Main'})
BottomCenter = boundary['Min']
BottomCenter[0] = (BottomCenter[0] + boundary['Max'][0])/2.0
BottomCenter[1] = (BottomCenter[1] + boundary['Max'][1])/2.0

offset = [0,0,0];
offset[0] = BuildplateCenter[0]-BottomCenter[0];
offset[1] = BuildplateCenter[1]-BottomCenter[1];
offset[2] = BuildplateCenter[2]-BottomCenter[2];

transformModel({'Name':'Main','Offset':offset})
transformModel({'Name':'Slice','Offset':offset})

print('Voxelize Model')
boundary = modelBoundary({'Name':'Main'})
voxelizeMin=boundary['Min']
voxelizeMin[0] = voxelizeMin[0] - boundary['Size'][0]/10.0;
voxelizeMin[1] = voxelizeMin[1] - boundary['Size'][1]/10.0;
voxelizeMin[2] = voxelizeMin[2] - boundary['Size'][2]/10.0;

voxelizeMax=boundary['Max']
voxelizeMax[0] = voxelizeMax[0] + boundary['Size'][0]/10.0;
voxelizeMax[1] = voxelizeMax[1] + boundary['Size'][1]/10.0;
voxelizeMax[2] = voxelizeMax[2] + boundary['Size'][2]/10.0;

resolution = [256,256,256]
voxelizeModel({
  'ModelName' : 'Main',
  'VoxelName' : 'Main',
  'Resolution': resolution,
  'Start'     : voxelizeMin,
  'End'       : voxelizeMax
});

print('Distance Map')

distanceMap({
    'VoxelName'   : 'Main',
    'DistanceMapName' : 'Main',
    'Resolution' : resolution,
    'Mode' : 'XY' # XY / XYZ
});
transformDistanceMap({
    'DistanceMapName' : 'Main',
    'VoxelName'   : 'Erosion',
    'SquaredDistance' : 2
});

print('Packing')
packVolume({
  'VoxelName':'Erosion',
  'DoubleName':'Main',
  'Resolution':resolution,
  'PackageSize':[2,2,2]
});

print('Triangulation')

triangulateDouble({
  'DoubleName':'Main',
  'ModelName' :'Triangulation',
  'Resolution':[resolution[0]/2,resolution[1]/2,resolution[1]/2],
  'Start'     :voxelizeMin,
  'End'       :voxelizeMax
});

saveModel({'Name':'Triangulation', 'Filename':'main.stl'})
print('Slice Model')
sliceModel({
    'ModelName'  : 'Triangulation',
    'ToolName'   : 'Slice',
    'Mode'       : 'Line',
    'LineName'   : 'Cut'
});

print('GCode Generation');

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
  'Line':'Cut',
  'Feedrate': 0.03
})

gcode += shutdown({});

saveText({
    'Text' : gcode,
    'Filename' : 'hello.gcode'
});

print('Finished script')