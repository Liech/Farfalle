print("Create Z Slice");

config      = getData({'Name':'config'});
SliceResolution = config['SliceResolution'];

Z = getData({'Name':'Z'});

boundary = modelBoundary({'Name':'Main'})
voxelizeMin=boundary['Min']
voxelizeMin[0] = voxelizeMin[0] - boundary['Size'][0]/10.0;
voxelizeMin[1] = voxelizeMin[1] - boundary['Size'][1]/10.0;
voxelizeMin[2] = voxelizeMin[2];

voxelizeMax=boundary['UniformMax']
voxelizeMax[0] = voxelizeMax[0] + boundary['UniformSize'][0]/10.0;
voxelizeMax[1] = voxelizeMax[1] + boundary['UniformSize'][1]/10.0;
voxelizeMax[2] = voxelizeMax[2];

print("  Generate Double Field...");
minZ = voxelizeMin[2]
maxZ = voxelizeMax[2]

isovalue = Z

print("  Isovalue: " + str(isovalue))

print("  Triangulate Slice...");
triangulateDouble({"DoubleName":"Slice","ModelName":"Slice","Resolution":SliceResolution,"Start":voxelizeMin,"End":voxelizeMax,"Isovalue":isovalue})

saveModel({"Name":"Slice","Filename":"slice/Slice" + "{:.2f}".format(Z) + ".stl"});
#simplifyModel({'Name':'Slice', "Ratio":0.6})
#saveModel({"Name":"Slice","Filename":"slice/SliceR" + "{:.2f}".format(Z) + ".stl"});
