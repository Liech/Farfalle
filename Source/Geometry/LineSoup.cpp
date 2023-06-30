#include "LineSoup.h"

#include "Model.h"

LineSoup::LineSoup(const Model& model) {
  size_t faces = model.getNumberFaces();

  for (size_t faceID = 0; faceID < faces; faceID++) {
    auto vertexIndices = model.getFaceIndices(faceID);
    glm::dvec3 p1 = model.getVertexPosition(vertexIndices[0]);
    glm::dvec3 p2 = model.getVertexPosition(vertexIndices[1]);
    glm::dvec3 p3 = model.getVertexPosition(vertexIndices[2]);

    soup.push_back(std::make_pair(p1, p2));
    soup.push_back(std::make_pair(p2, p3));
    soup.push_back(std::make_pair(p3, p1));
  }

}

std::vector<std::pair<glm::dvec2, glm::dvec2>> LineSoup::segmentCut(glm::dvec2 start, glm::dvec2 end) {
  return {};
}

void LineSoup::save(const std::string& filename) {

}

