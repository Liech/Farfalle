#include "Volume.h"

#include "VolumeImplementation.h"
#include "Model.h"


Volume::Volume(std::unique_ptr<VolumeImplementation> impl) {
  p = std::move(impl);
}

Volume::~Volume() {

}

std::unique_ptr<Model> Volume::getModel() const {
  return p->getModel();
}