print("Slice Z");

config      = getData({'Name':'config'});
layerHeight = config['LayerHeight'];

print('Prepare Slice Region')
boundary = modelBoundary({'Name':'Main'})
sliceMin=boundary['Min']
sliceMin[0] = sliceMin[0] - boundary['Size'][0]/10.0;
sliceMin[1] = sliceMin[1] - boundary['Size'][1]/10.0;
sliceMin[2] = sliceMin[2] - boundary['Size'][2]/10.0;

sliceMax=boundary['Max']
sliceMax[0] = sliceMax[0] + boundary['Size'][0]/10.0;
sliceMax[1] = sliceMax[1] + boundary['Size'][1]/10.0;
sliceMax[2] = sliceMax[2] + boundary['Size'][2]/10.0;

Z = 0.2
counter = 0;


setData({'Name':'ZCounter', 'Data' : 0});

while(Z < sliceMax[2]):
  print('Z Layer ' + str(counter));
  createPlane({
    'Name' : 'Slice',
    'Origin' : [sliceMin[0],sliceMin[1],Z],
    'Normal' : [0,0,1],
    'Size' : 5000
  });
  executePythonFile({'Filename' : 'SliceOuter.py'});
  executePythonFile({'Filename' : 'SliceInner.py'});

  Z = Z+layerHeight;
  counter = counter+1;
 