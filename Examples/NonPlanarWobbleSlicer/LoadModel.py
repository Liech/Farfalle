import os
import shutil

print("Load Model");

config = getData({'Name':'config'});
Filename         = config['Filename'];
BuildplateCenter = config['BuildplateCenter'];
Detail = config['Detail'];
IO = config['IntermediateFiles'];

if (IO):
  if os.path.exists('dbg') and os.path.isdir('dbg'):
    shutil.rmtree('dbg')
  os.mkdir('dbg')
  os.mkdir('dbg/slice');

loadModel({'Name': 'Main', 'Filename':Filename})
repairModel({'Name':'Main'})
#if (Detail < 1):
simplifyModel({'Name':'Main', "Ratio":0.1})

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
if (IO):
  saveModel({"Name":"Main","Filename":"dbg/Main.stl"});
print('Load finished')