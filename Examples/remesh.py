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

voxelizeModel({
  'ModelName' : 'Main',
  'VoxelName' : 'Main',
  'Resolution': [256,256,256],
  'Start'     : voxelizeMin,
  'End'       : voxelizeMax
});

print('Triangulation')

triangulateVolume({
  'VoxelName' :'Main',
  'ModelName' :'Triangulation',
  'Resolution':[256,256,256],
  'Start'     :voxelizeMin,
  'End'       :voxelizeMax
});

print('Save Model');
saveModel({'Name':'Triangulation', 'Filename':'../dbg/main.stl'})

print('Finished script')
