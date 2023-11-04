import math

print("Create Z Density Field");

config      = getData({'Name':'config'});
SliceResolution = config['SliceResolution'];

Z = getData({'Name':'Z'});

boundary = modelBoundary({'Name':'Main'})
voxelizeMin=boundary['Min']
voxelizeMin[0] = voxelizeMin[0] - boundary['Size'][0]/10.0;
voxelizeMin[1] = voxelizeMin[1] - boundary['Size'][1]/10.0;

voxelizeMax=boundary['UniformMax']
voxelizeMax[0] = voxelizeMax[0] + boundary['UniformSize'][0]/10.0;
voxelizeMax[1] = voxelizeMax[1] + boundary['UniformSize'][1]/10.0;

print("  Generate Double Field...");
minZ = voxelizeMin[2]
maxZ = voxelizeMax[2]

xWobble = 1.5
zWobble = 0.5
print("minZ "+str(minZ))
def density(coord): 
  distortion = (math.sin(coord[1]/xWobble)*zWobble + math.cos(coord[0]/xWobble)*zWobble);
  perc = (coord[2] / SliceResolution[2])
  distance = abs(perc-0.1)
  distortionInfluence = 0
  R = 0.1
  if (distance < R):
    distortionInfluence = ((R-distance)/R)
  return (maxZ-minZ)*perc + distortion * distortionInfluence
  
print("  createDensityField...");
createDensityField({"Name":"Slice", "Resolution":SliceResolution, "Language":"Python","Function":density})
