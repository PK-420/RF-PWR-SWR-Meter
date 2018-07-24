#include "Meter.h"

Meter::Meter(int meterPin) {
  _pin = meterPin;
}

double Meter::vcc() { // outputs Vcc in V
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  return result/1000.0; // returns answer in Volts
}

double Meter::voltage() {
  double Vcc = vcc();
  double volts = (analogRead(_pin) / 1024.0) * Vcc;
  delay(2);
  return volts;
}

double Meter::vAvg(int samples = 10) {
  double vAvg = 0;
  for (int i = samples; i > 0; i--) vAvg = vAvg + voltage();
  return vAvg / (double)samples;
}
