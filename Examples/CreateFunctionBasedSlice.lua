print("Creating Density Field")
function density(coord)
  return  (coord[2] / 128.0) + math.sin(coord[1])*0.01 + math.cos(coord[0])*0.01
end

local resolution = {128,128,128}
createDensityField({Name="Main", Resolution=resolution, Language="Lua",Function=density})

print("Triangulate")
triangulateDouble({DoubleName="Main",ModelName="Main",Resolution=resolution,Start={0,0,0},End={1,1,1},Isovalue=0.5})

print("Save");
saveModel({Name="Main",Filename="density.stl"});
