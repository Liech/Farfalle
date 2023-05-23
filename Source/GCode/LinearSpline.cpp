#include "LinearSpline.h"

namespace GCode
{
  std::string LinearSpline::toString() const
  {
    //std::string result;
    //for (auto& point : controlPoints)
    //{
    //  result += "G1 X" + std::to_string(point[0]) + " Y" + std::to_string(point[1]) + " Z" + std::to_string(point[2]) + " E" + std::to_string(feedrate) + "\n";
    //}

    return
R"(
G1 X60 Y61.3 
G1 Z.2
G1 E2 F1800
M204 S1000
G1 F900
G1 X60 Y58.7 E.0772
M204 S3000
G1 E-2 F3000
G1 Z.6 F18000
)";
  }
}