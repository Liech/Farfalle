
config      = getData({'Name':'config'});
NozzleDiameter = config['NozzleDiameter'];


sliceModel({
    'ModelName'    : 'Main',
    'ToolName'     : 'Slice',
    'Mode'         : 'Model',
    'ResultName'   : 'SliceZ'
});

boundary = modelBoundary({'Name':'SliceZ'})
sliceMin=boundary['Min']
sliceMin[0] = sliceMin[0] - boundary['Size'][0]/10.0;
sliceMin[1] = sliceMin[1] - boundary['Size'][1]/10.0;
sliceMin[2] = sliceMin[2] - boundary['Size'][2]/10.0;

sliceMax=boundary['Max']
sliceMax[0] = sliceMax[0] + boundary['Size'][0]/10.0;
sliceMax[1] = sliceMax[1] + boundary['Size'][1]/10.0;
sliceMax[2] = sliceMax[2] + boundary['Size'][2]/10.0;

print('  SliceY')



print('  Slice X')
streaks = []
streakCount = 0;



X = sliceMin[0];
counter = 0;
while(X < sliceMax[0]):
  createPlane({
      'Name' : 'SliceX',
      'Origin' : [X,0,0.2],
      'Normal' : [1,0,0],
      'Size' : 5000
  });
  streakName = 'Streak' + str(counter);
  sliceModel({
      'ModelName'  : 'SliceZ',
      'ToolName'   : 'SliceX',
      'Mode'       : 'Line',
      'LineName'   : streakName
  });

  streaks.append(streakName);

  X = X + NozzleDiameter;
  counter = counter+1;
  

zCounter = getData({'Name':'ZCounter'});
mergeLines({
    'Result':'Result' + str(zCounter),
    'Radius' : 0.5,
    'Input' : streaks
});
setData({'Name':'ZCounter', 'Data' : zCounter+1});
