from PolyglotModule import *

config = {
  'Filename':"3DBenchy.stl",
  'ResultFilename' : '3DBenchy.gcode',
  'BuildplateCenter' : [100,100,0],
  'LayerHeight' : 0.2,
  'NozzleDiameter' : 0.4,
  'VoxelResolution':[1024,1024,1024],
  'SliceResolution':[32,32,32],
  'WallThickness' : 0.4,
  'PlanarSlicing' : True,
  'LayerLimit' : 6
};

setData({'Name':'config', 'Data' : config });

executePythonFile({'Filename' : 'LoadModel.py'}); # Defines Model 'Main'
if (config["PlanarSlicing"] == False):
  executePythonFile({'Filename' : 'BuildZSliceField.py'});
executePythonFile({'Filename' : 'Categorize.py'}); # Defines Model 'InnerArea' and 'OuterArea'
executePythonFile({'Filename' : 'PrepareWall.py'});
executePythonFile({'Filename' : 'SliceZ.py'}); # Defines Lines 'Result0'...'ResultN' [N = getData({'ZCounter'})
executePythonFile({'Filename' : 'WriteGCode.py'});


print('Finished script')