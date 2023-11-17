from PolyglotModule import *
print("Prepare Config");
config = {
  'Filename':"3DBenchy.stl",
  'ResultFilename' : '3DBenchy.gcode',
  'BuildplateCenter' : [100,100,0],
  'LayerHeight' : 0.2,
  'WallAmount' : 2,
  'NozzleDiameter' : 0.43,
  'VoxelResolution':[512,512,512],
  'SliceResolution':[32,32,32],
  'WallThickness' : 0.4,
  'PlanarSlicing' : True,
  'LayerLimit' : -1, # -1 for unlimited
  'Detail' : 0.3, #0.0-1.0
  'IntermediateFiles' : False 
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

print("Start:" + 'LoadModel.py');
executePythonFile({'Filename' : 'LoadModel.py'}); # Defines Model 'Main'
if (config["PlanarSlicing"] == False):
  print("Start:" + 'BuildZSliceField.py');
  executePythonFile({'Filename' : 'BuildZSliceField.py'});
print("Start:" + 'Categorize.py');
executePythonFile({'Filename' : 'Categorize.py'}); # Defines Model 'InnerArea' and 'OuterArea'
print("Start:" + 'SliceZ.py');
executePythonFile({'Filename' : 'SliceZ.py'}); # Defines Lines 'Result0'...'ResultN' [N = getData({'ZCounter'})
print("Start:" + 'WriteGCode.py');
executePythonFile({'Filename' : 'WriteGCode.py'});


print('Finished script')