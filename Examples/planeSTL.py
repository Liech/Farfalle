
createPlane({
    'Name' : 'SliceX',
    'Origin' : [5,0,0.2],
    'Normal' : [1,0,0],
    'Size' : 5000
});

saveModel({
  'Name' : 'SliceX',
  'Filename': 'main.stl'
});

