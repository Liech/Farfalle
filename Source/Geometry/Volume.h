#pragma once

#include <memory>

class VolumeImplementation;
class Model;

class Volume {
public:
  Volume(std::unique_ptr<VolumeImplementation>);
  virtual ~Volume();

  std::unique_ptr<Model> getModel() const;

  void minkowski(Volume&);

private:
  std::unique_ptr<VolumeImplementation> p;
};