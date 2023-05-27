#include "Printer.h"

#include <algorithm>

#include "Extruder.h"
#include "Temperature.h"
#include "Movement.h"

Printer::Printer() {
  extruder    = std::make_unique<Extruder>(*this);
  temperature = std::make_unique<Temperature>();
  movement    = std::make_unique<Movement>(*this);
}

void Printer::startup(std::string& result)
{
  result += "\n; ;;;;;;;;;;;;;;;;;;";
  result += "\n; Printer Startup:  ";
  result += "\n; ;;;;;;;;;;;;;;;;;;\n";

  result += "G90 ; absolute coords\n";
  result += "G21 ; units to millimeter\n";

  temperature->fanOff(result);
  temperature->startHeating(result);
  movement   ->startup(result);
  extruder   ->startup(result);
  movement   ->home(result);
  temperature->waitForHeating(result);
  extruder   ->prime(result);
}

void Printer::shutdown(std::string& result)
{
  result += "\n; ;;;;;;;;;;;;;;;;;;";
  result += "\n; Printer Shutdown: ";
  result += "\n; ;;;;;;;;;;;;;;;;;;\n";
  temperature->shutdown(result);
  movement   ->shutdown(result);
}

