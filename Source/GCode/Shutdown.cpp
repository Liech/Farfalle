#include "Shutdown.h"

namespace GCode
{
  std::string Shutdown::toString() const
  {
    return
R"(
M107      ;Shutdown Fans
print_end    ;end script from macro
)";
  }
}