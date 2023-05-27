#pragma once

#include <string>

class Temperature
{
public:
  double bed      = 60;
  double extruder = 205;

  void startHeating  (std::string& result) const;
  void waitForHeating(std::string& result) const;
  void shutdown      (std::string& result) const;
  void fanOff        (std::string& result) const;
};