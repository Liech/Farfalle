from PolyglotModule import *

config = {
  'Filename':"3DBenchy.stl",
  'ResultFilename' : '3DBenchy.gcode',
  'BuildplateCenter' : [100,100,0],
  'LayerHeight' : 0.2,
  'NozzleDiameter' : 0.4,
  'VoxelResolution':[1024,1024,1024],
  'SliceResolution':[64,64,64],
  'WallThickness' : 0.4,
  
};

setData({'Name':'config', 'Data' : config });

executePythonFile({'Filename' : 'LoadModel.py'}); # Defines Model 'Main'
executePythonFile({'Filename' : 'Categorize.py'}); # Defines Model 'InnerArea' and 'OuterArea'
executePythonFile({'Filename' : 'PrepareWall.py'});
executePythonFile({'Filename' : 'BuildZSliceField.py'});
executePythonFile({'Filename' : 'SliceZ.py'}); # Defines Lines 'Result0'...'ResultN' [N = getData({'ZCounter'})
executePythonFile({'Filename' : 'WriteGCode.py'});


print('Finished script')