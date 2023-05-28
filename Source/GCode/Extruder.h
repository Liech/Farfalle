#pragma once

#include <string>

#include "Printer.h"

class Extruder {
public:
  double position                 = 0;
  double accelerationLimits       = 5000;
  double maxFeedrate              = 80;
  double jerkLimit                = 10;
  double flow                     = 95;
  double linearAdvance            = 0.2;
  double startPrintHeight         = 0.2;
  double minimumExtrudingFeedRate = 0;
  double maximumFeedrate          = 0.5;

  Extruder(Printer&);
  void startup(std::string& result);
  void prime  (std::string& result);
  void retract(std::string& result);
  void unretract(std::string& result);
  void move   (std::string& result, double offset);

private:
  Printer& printer;
};