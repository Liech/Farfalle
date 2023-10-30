print("Hello world")
function density(coord)
  return (coord[2] / 128.0)
end
local resolution = {128,128,128}
createDensityField({Name="Main", Resolution=resolution, Language="Lua",Function=density})
