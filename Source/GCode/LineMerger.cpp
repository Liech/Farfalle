#include "LineMerger.h"

#include <stdexcept>
#include <set>
#include <map>
#include "Geometry/KDTree.h"


LineMerger::LineMerger() {

}

std::vector<std::vector<glm::dvec3>> LineMerger::merge(const std::vector<std::vector<glm::dvec3>>& input, double mergeDistance) {
  std::vector<glm::dvec3> startPoints;
  std::vector<glm::dvec3> endPoints  ;

  for (auto& x : input) {
    if (x.size() > 0) {
      startPoints.push_back(x[0]);
      endPoints.push_back(x[x.size() - 1]);
    }
  }

  std::vector<glm::dvec3> allPoints;
  allPoints.insert(allPoints.begin(),startPoints.begin(), startPoints.end());
  allPoints.insert(allPoints.begin(),  endPoints.begin(),   endPoints.end());
  KDTree tree(allPoints);

  std::set<size_t> todo;
  for (size_t i = 0; i < allPoints.size(); i++)
    todo.insert(i);

  size_t splitPoint = startPoints.size();
  auto isLineEnd = [splitPoint](size_t index) { return splitPoint >= index; };
  auto getLineIndex = [splitPoint](size_t index) { if (index >= splitPoint) return index - splitPoint; else return index; };

  struct merge {
    size_t lineA;
    size_t lineB;
    bool   endA; //which end of the line is merged with which? false: Start; end: True
    bool   endB; //which end of the line is merged with which? false: Start; end: True
  };

  std::vector<merge> merges;
  while (todo.size() > 0) {
    size_t current = *todo.begin();
    todo.erase(current);
    tree.remove(current);

    auto neighbours = tree.find(allPoints[current], mergeDistance); //sorted by distance
    if (neighbours.size() > 0) {
      size_t mergeWith = neighbours[0];
      merge sub;
      sub.endA  = isLineEnd(current);
      sub.endB  = isLineEnd(mergeWith);
      sub.lineA = getLineIndex(current);
      sub.lineB = getLineIndex(mergeWith);
      if (sub.lineA != sub.lineB) {
        merges.push_back(sub);
        todo.erase(mergeWith);
        tree.remove(mergeWith);
      }
    }
  }

  std::vector<std::vector<glm::dvec3>> result = input;
  std::map<size_t, size_t> forwarding;

  for (merge& m : merges) {
    size_t iA = m.lineA;
    size_t iB = m.lineB;
    std::vector<glm::dvec3>* a;
    std::vector<glm::dvec3>* b;

    while (forwarding.count(iA) != 0) {
      iA = forwarding[iA];
    }
    while (forwarding.count(iB) != 0) {
      iB = forwarding[iB];
    }
    a = &result[iA];
    b = &result[iB];
    if (iA == iB)
      continue;

    assert(a->size() != 0);
    assert(b->size() != 0);

    //S: Start, E: End;
    double SS = glm::distance(a->at(0)            , b->at(0            ));
    double SE = glm::distance(a->at(0)            , b->at(b->size() - 1));
    double ES = glm::distance(a->at(a->size() - 1), b->at(0            ));
    double EE = glm::distance(a->at(a->size() - 1), b->at(b->size() - 1));

    if (SS < SE && SS < ES && SS < EE) { //SS smallest
      std::reverse(b->begin(), b->end());
      a->insert(a->begin(), b->begin(), b->end());
    }
    else if (SE < ES && SE < EE) { //SE smallest
      a->insert(a->begin(), b->begin(), b->end());
    }
    else if (ES < EE) { //ES smallest
      a->insert(a->end(), b->begin(), b->end());
    }
    else { //if (EE smallest)
      std::reverse(b->begin(), b->end());
      a->insert(a->end(), b->begin(), b->end());
    }
    forwarding[iB] = iA;
    
    b->clear();
  }

  for (int i = result.size(); i >= 0; i--)
    if (result[i].size() == 0)
      result.erase(result.begin() + i);

  return result;
}