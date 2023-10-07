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

voxelizeMax=boundary['UniformMax']
voxelizeMax[0] = voxelizeMax[0] + boundary['UniformSize'][0]/10.0;
voxelizeMax[1] = voxelizeMax[1] + boundary['UniformSize'][1]/10.0;
voxelizeMax[2] = voxelizeMax[2] + boundary['UniformSize'][2]/10.0;

resolution = [512,512,512]
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

voxelSize = boundary["UniformSize"][0] / resolution[0];
print('Voxel Size: ' + str(voxelSize))

nozzlediam = 0.4
eroded = 0.0;

streaks = []
streakCount = 0;

while(True):
  distance = (eroded/voxelSize)
  print('Erosion: ' + str(distance));
  transformDistanceMap({
      'DistanceMapName' : 'Main',
      'VoxelName'   : 'Erosion',
      'Distance' : distance
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
  
  hasTri = hasAnyTriangle({'Name':'Triangulation'});
  if (not hasTri):
    break;
  
  saveModel({'Name':'Triangulation', 'Filename':'main.stl'})
  
  streakname = "Streak" + str(streakCount);
  streaks.append(streakname);
  streakCount+=1;
  
  print('Slice Model')
  sliceModel({
      'ModelName'  : 'Triangulation',
      'ToolName'   : 'Slice',
      'Mode'       : 'Line',
      'LineName'   : streakname
  });
  eroded+=nozzlediam
 

print('GCode Generation');

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

for streak in streaks:
  gcode += linearPrint({
    'Line':streak,
    'Feedrate': 0.03
  })

gcode += shutdown({});

saveText({
    'Text' : gcode,
    'Filename' : 'hello.gcode'
});

print('Finished script')