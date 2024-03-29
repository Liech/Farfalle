#include "Temperature.h"


void Temperature::startHeating(std::string& result)   const
{
  result += "\n; Start Heating: \n";
  result += "M104 S" + std::to_string(extruder) + " ; extruder temp\n";
  result += "M140 S" + std::to_string(bed) + " ; bed temp\n";
}

void Temperature::waitForHeating(std::string& result) const
{
  result += "\n; Wait for Heating: \n";
  result += "M109 R" + std::to_string(extruder) + " ; wait for extruder temp\n";
  result += "M190 S" + std::to_string(bed) + " ; wait for bed temp\n";
}

void Temperature::shutdown(std::string& result) const
{
  result += "\n; Temperature Shutdown: \n";
  result += "M104 S0 ; extruder temp off\n";
  result += "M140 S0 ; bed temp off\n";
}

void Temperature::fanOff(std::string& result) const
{
  result += "M107 ; fan off\n";
}