#include "LineMerger.h"

#include <stdexcept>
#include <set>
#include <map>
#include <algorithm>
#include "Geometry/KDTree.h"


LineMerger::LineMerger() {

}

double LineMerger::getLength(const std::vector<glm::dvec3>& streak) {
  double result = 0;
  for (size_t i = 1; i < streak.size(); i++) {
    auto& prev = streak[i - 1];
    auto& current = streak[i];
    result += glm::distance(prev, current);
  }
  return result;
}

std::vector<std::vector<glm::dvec3>> LineMerger::merge(const std::vector<std::vector<glm::dvec3>>& input, double mergeDistance) {
  std::vector<glm::dvec3> startPoints;
  std::vector<glm::dvec3> endPoints;

  for (auto& x : input) {
    if (x.size() > 0) {
      startPoints.push_back(x[0]);
      endPoints.push_back(x[x.size() - 1]);
    }
  }

  std::vector<glm::dvec3> allPoints;
  allPoints.insert(allPoints.begin(), startPoints.begin(), startPoints.end());
  allPoints.insert(allPoints.begin(), endPoints.begin(), endPoints.end());
  KDTree tree(allPoints);

  size_t splitPoint = startPoints.size();
  auto isLineEnd = [splitPoint](size_t index) { return splitPoint >= index; };
  auto getLineIndex = [splitPoint](size_t index) { if (index >= splitPoint) return index - splitPoint; else return index; };

  std::set<size_t> todo;
  for (size_t i = 0; i < input.size(); i++)
    todo.insert(i);

  std::vector<std::vector<glm::dvec3>> result = input;
  while (todo.size() > 0) {
    std::vector<size_t> longest(todo.begin(),todo.end());
    std::sort(longest.begin(), longest.end(), [&](const size_t& a, const size_t& b)
      {
        double lenA = getLength(input[a]);
        double lenB = getLength(input[b]);
        return lenA > lenB;
      });
    size_t current = longest[0];
    todo.erase(current);

    auto& currentList = result[current];
    if (currentList.size() == 0)
      continue;
    bool workDone = true;
    while (workDone) {
      workDone = false;
      glm::dvec3 start = currentList[0];
      glm::dvec3 end = currentList[currentList.size() - 1];

      auto candidatesStart = tree.find(start, mergeDistance);
      for (auto candidate : candidatesStart) {
        if (glm::distance(allPoints[candidate], start) > mergeDistance)
          continue;          
        size_t index = getLineIndex(candidate);
        bool atEnd = isLineEnd(candidate);
        auto& candidateList = result[index];
        if (todo.count(index) == 0)
          continue;
        todo.erase(index);
        if (!atEnd)
          std::reverse(candidateList.begin(), candidateList.end());        
        currentList.insert(currentList.begin(), candidateList.begin(), candidateList.end());
        candidateList.clear();
        workDone = true;
        break;
      }

      //auto candidatesEnd = tree.find(end, mergeDistance);
      //for (auto candidate : candidatesEnd) {
      //  size_t index = getLineIndex(candidate);
      //  bool atEnd = isLineEnd(candidate);
      //  auto& candidateList = result[index];
      //  if (todo.count(index) == 0)
      //    continue;
      //  todo.erase(index);        
      //  if (atEnd)
      //    std::reverse(candidateList.begin(), candidateList.end());
      //  currentList.insert(currentList.end(), candidateList.begin(), candidateList.end());
      //  candidateList.clear();
      //  workDone = true;
      //  break;
      //}
    }

  }

  for (int i = result.size()-1; i >= 0; i--)
    if (result[i].size() == 0)
      result.erase(result.begin() + i);
  return result;
}