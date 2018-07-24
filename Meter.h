#ifndef METER_H
#define METER_H

#include "Arduino.h"

class Meter {
  private:
    int _pin;
  public:
    Meter(int pin);
    double vcc();
    double voltage();
    double vAvg(int samples = 10);
};

double dBm2mW(float dBm);

#endif



