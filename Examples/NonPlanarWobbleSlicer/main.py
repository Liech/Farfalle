from PolyglotModule import *

config = {
  'Filename':"3DBenchy.stl",
  'ResultFilename' : '3DBenchy.gcode',
  'BuildplateCenter' : [100,100,0],
  'LayerHeight' : 0.2,
  'NozzleDiameter' : 0.43,
  'VoxelResolution':[1024,1024,1024],
  'SliceResolution':[32,32,32],
  'WallThickness' : 0.4,
  'PlanarSlicing' : False,
  'LayerLimit' : -1,
  'Detail' : 0.1 #0.0-1.0
};

#createPlane({
#    'Name' : 'ground',
#    'Origin' : config["BuildplateCenter"],
#    'Normal' : [0,0,1],
#    'Size' : 100
#});
#saveModel({"Name":"ground","Filename":"ground.stl"});
#createPlane({
#    'Name' : 'firstLayer',
#    'Origin' : [config["BuildplateCenter"][0],config["BuildplateCenter"][1],0.2],
#    'Normal' : [0,0,1],
#    'Size' : 100
#});
#saveModel({"Name":"firstLayer","Filename":"firstLayer.stl"});

setData({'Name':'config', 'Data' : config });

setData({'Name':'ZCounter', 'Data' : 0});

executePythonFile({'Filename' : 'LoadModel.py'}); # Defines Model 'Main'
if (config["PlanarSlicing"] == False):
  executePythonFile({'Filename' : 'BuildZSliceField.py'});
executePythonFile({'Filename' : 'Categorize.py'}); # Defines Model 'InnerArea' and 'OuterArea'
executePythonFile({'Filename' : 'PrepareWall.py'});
executePythonFile({'Filename' : 'SliceZ.py'}); # Defines Lines 'Result0'...'ResultN' [N = getData({'ZCounter'})
executePythonFile({'Filename' : 'WriteGCode.py'});


print('Finished script')