#include "String2File.h"

#include <fstream>
#include <string>
#include <iostream>

namespace Tools
{
  void String2File(const std::string& filename, const std::string& data)
  {
    std::ofstream out(filename);
    out << data;
    out.close();
  }
}