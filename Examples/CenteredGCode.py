filename = "C:\\Users\\nicol\\Downloads\\_3DBenchy_-_The_jolly_3D_printing_torture-test_by_CreativeTools_se_763622\\files\\3DBenchyFixed.stl";
slice = "C:\\Users\\nicol\\Documents\\GitHub\\Farfalle\\out\\dbg\\slice0.stl"

BuildplateCenter = [100,100,0]

gcode = ""

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

print('Slice Model')
sliceModel({
    'ModelName'  : 'Main',
    'ToolName'   : 'Slice',
    'Mode'       : 'Line',
    'LineName'   : 'Cut'
});

print('GCode Generation');


gcode += linearPrint({
  'Line':'Cut',
  'Feedrate': 0.03
})

saveText({
    'Text' : gcode,
    'Filename' : 'hello.gcode'
});

print('Finished script')