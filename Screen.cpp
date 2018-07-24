#include "Screen.h"

Screen::Screen() {
  
}

Screen::Screen(String title, String unit, double value = 0) {
  _params = 1;
  _title = title;
  _units = new String[_params];
  _values = new double[_params];
  _decimals = new int[_params];
  _steps = new double[_params];
  _mins = new double[_params];
  _maxs = new double[_params];
  for (int i = 0; i < _params; i++) {
    _steps[i] = 0;
    _decimals[i] = 0;
    _values[i] = 0;
    _units[i] = "";
    _mins[i] = 0;
    _maxs[i] = 0;
  }
  _values[0] = value;
  _units[0] = unit;
}

Screen::Screen(String title, String unit1, String unit2, double value1 = 0, double value2 = 0) {
  _params = 2;
  _title = title;
  _units = new String[_params];
  _values = new double[_params];
  _decimals = new int[_params];
  _steps = new double[_params];
  _mins = new double[_params];
  _maxs = new double[_params];
  for (int i = 0; i < _params; i++) {
    _steps[i] = 0;
    _decimals[i] = 0;
    _values[i] = 0;
    _units[i] = "";
    _mins[i] = 0;
    _maxs[i] = 0;
  }
  _values[0] = value1;
  _units[0] = unit1;
  _values[1] = value2;
  _units[1] = unit2;
}

Screen::Screen(String title, String unit1, String unit2, String unit3, double value1 = 0, double value2 = 0, double value3 = 0) {
  _params = 3;
  _title = title;
  _units = new String[_params];
  _values = new double[_params];
  _decimals = new int[_params];
  _steps = new double[_params];
  _mins = new double[_params];
  _maxs = new double[_params];
  for (int i = 0; i < _params; i++) {
    _steps[i] = 0;
    _decimals[i] = 0;
    _values[i] = 0;
    _units[i] = "";
    _mins[i] = 0;
    _maxs[i] = 0;
  }
  _values[0] = value1;
  _units[0] = unit1;
  _values[1] = value2;
  _units[1] = unit2;
  _values[2] = value3;
  _units[2] = unit3;
}

void Screen::changeTitle(String title) {
  _title = title;
}

void Screen::changeUnit(String unit, int i = 0) {
  _units[i] = unit;
}

void Screen::changeValue(double value, int i = 0) {
  _values[i] = value;
}

void Screen::setDecimalPoint(int dp, int i = 0) {
  _decimals[i] = dp;
}

void Screen::setStep(double s, int i = 0) {
  _steps[i] = s;
}

void Screen::increment(int i = 0) {
  if (_values[i] < _maxs[i]) _values[i] += _steps[i];
  else _values[i] = _maxs[i];
}
void Screen::decrement(int i = 0) {
  if (_values[i] > _mins[i])_values[i] -= _steps[i];
  else _values[i] = _mins[i];
}

void Screen::setMin(double limit, int i = 0) {
  _mins[i] = limit;
}
void Screen::setMax(double limit, int i = 0) {
  _maxs[i] = limit;
}

void Screen::setLimits(double low, double high, int i = 0) {
  _mins[i] = low;
  _maxs[i] = high;
}

String Screen::title() {
  return _title;
}

String Screen::unit(int i = 0) {
  return _units[i];
}

double Screen::value(int i = 0) {
  return _values[i];
}

int Screen::decimalPoint(int i = 0) {
  return _decimals[i];
}

int Screen::params() {
  return _params;
}

