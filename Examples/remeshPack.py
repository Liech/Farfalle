filename = "C:\\Users\\nicol\\Downloads\\_3DBenchy_-_The_jolly_3D_printing_torture-test_by_CreativeTools_se_763622\\files\\3DBenchyFixed.stl";

print('Load Model')
loadModel({'Name': 'Main', 'Filename':filename})

print('Repair Model')
repairModel({'Name':'Main'})

print('Voxelization')
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
deleteModel({'Name' : 'Main'});

print('Packing')
packVolume({
  'VoxelName':'Main',
  'DoubleName':'Main',
  'Resolution':resolution,
  'PackageSize':[2,2,2]
});

print('Triangulation')

triangulateDouble({
  'DoubleName' :'Main',
  'ModelName' :'Triangulation',
  'Resolution':[resolution[0]/2,resolution[1]/2,resolution[1]/2],
  'Start'     :voxelizeMin,
  'End'       :voxelizeMax
});
deleteVolume({'Name' : 'Main'})

print('Save Model');
saveModel({'Name':'Triangulation', 'Filename':'main.stl'})
deleteDouble({'Name' : 'Main'})
deleteModel({'Name' : 'Triangulation'})

print('Finished script')
