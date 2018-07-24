#ifndef SCREEN_H
#define SCREEN_H
#include "Arduino.h"

class Screen {
  private:
    int _params;
    String _title = "";
    String* _units;
    double* _values;
    int* _decimals;
    double* _steps;
    double* _mins;
    double* _maxs;
  public:
    Screen();
    Screen(String title);
    Screen(String title, String unit, double value = 0);
    Screen(String title, String unit1, String unit2, double value1 = 0, double value2 = 0);
    Screen(String title, String unit1, String unit2, String unit3, double value1 = 0, double value2 = 0, double value3 = 0);
    void changeTitle(String title);
    void changeUnit(String unit, int i = 0);
    void changeValue(double value, int i = 0);
    void setDecimalPoint(int dp, int i = 0);
    void setStep(double s, int i = 0);
    void setMin(double limit, int i = 0);
    void setMax(double limit, int i = 0);
    void setLimits(double low, double high, int i = 0);
    void increment(int i = 0);
    void decrement(int i = 0);
    String title();
    String unit(int i = 0);
    double value(int i = 0);
    int decimalPoint(int i = 0);
    int params();
};

#endif
