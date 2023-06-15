#include "glm2svg.h"

#include "String2File.h"

namespace Tools {
  void glm2svg::save(const std::string& filename, const std::vector<std::vector<glm::dvec2>>& loop) {
    std::string data = "<svg viewBox=\"0 0 100 100\" xmlns=\"http://www.w3.org/2000/svg\">\n";

    for (auto& streak : loop) {
      
      for (size_t i = 1; i < streak.size(); i++) {

        glm::dvec2 start = streak[i-1];
        glm::dvec2 end   = streak[i  ];
        data += "<line x1 = \"" + std::to_string(start.x) + "\" y1 = \"" + std::to_string(start.y) + "\" x2 = \"" + std::to_string(end.x) + "\" y2 = \"" + std::to_string(end.x) + "\" stroke = \"black\" />\n";
      }
    }

    data += "\n<\svg>";
    Tools::String2File(filename, data);
  }
}