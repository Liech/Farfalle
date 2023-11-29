import math

print("Create Z Density Field");

config      = getData({'Name':'config'});
SliceResolution = config['SliceResolution'];
WobbleAmplitude = config['WobbleAmplitude'];
WobbleFrequency = config['WobbleFrequency'];
WobbleFrom      = config['WobbleFrom'];
WobbleTo        = config['WobbleTo'];
rampTime        = config['rampTime'];

Z = getData({'Name':'Z'});

boundary = modelBoundary({'Name':'Main'})
voxelizeMin=boundary['Min']
voxelizeMin[0] = voxelizeMin[0] - boundary['UniformSize'][0]/10;
voxelizeMin[1] = voxelizeMin[1] - boundary['UniformSize'][1]/10;

voxelizeMax=boundary['UniformMax']
voxelizeMax[0] = voxelizeMax[0] + boundary['UniformSize'][0]/10;
voxelizeMax[1] = voxelizeMax[1] + boundary['UniformSize'][1]/10;

print("  Generate Double Field...");
minZ = voxelizeMin[2]
maxZ = voxelizeMax[2]

data = {};
data["minZ"] = minZ;
data["maxZ"] = maxZ;
data["SliceResolution"] = SliceResolution;
data["WobbleAmplitude"] = WobbleAmplitude;
data["WobbleFrequency"] = WobbleFrequency;
data["WobbleFrom"]      = WobbleFrom;
data["WobbleTo"]        = WobbleTo  ;
data["rampTime"]        = rampTime  ;

print("minZ "+str(minZ))
def density(input): 
  import math
  coord = input["Pos"]
  data = input["Data"]
  xWobble = data["WobbleFrequency"]
  zWobble = data["WobbleAmplitude"]
  #distortion = (math.sin(coord[1]*(xWobble/data["SliceResolution"][0]))*zWobble + math.cos(coord[0]*(xWobble/data["SliceResolution"][0]))*zWobble);
  distortion = (math.sin(coord[0]*(xWobble/data["SliceResolution"][0]) + 3.14/2)*zWobble)*zWobble;
  perc = (coord[2] / data["SliceResolution"][2])
  distortionInfluence = 0
  fromPercentage = data["WobbleFrom"]
  toPercentage   = data["WobbleTo"]
  rampTime       = data["rampTime"]
  distanceFrom = abs(perc-fromPercentage)
  distanceTo   = abs(perc-toPercentage)
  if (perc > fromPercentage and perc < toPercentage):
    distortionInfluence = 1
  elif(distanceFrom < rampTime):
    distortionInfluence = ((rampTime-distanceFrom)/rampTime)
  elif(distanceTo < rampTime):
    distortionInfluence = ((rampTime-distanceTo)/rampTime)
  return (data["maxZ"]-data["minZ"])*perc + distortion * distortionInfluence
  
print("  createDensityField...");
createDensityField({"Name":"Slice", "Resolution":SliceResolution, "Language":"Python","Function":density, "Data":data})
