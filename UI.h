#ifndef UI_H
#define UI_H
#include "Arduino.h"
#include <LiquidCrystal.h>
#include "Screen.h"

class UI {
  private:
    int currentMode;
    byte _dBlogo[8] = {
      B00110,
      B00101,
      B01101,
      B10110,
      B10101,
      B10101,
      B01110,
      B00000,
    };
    LiquidCrystal *_lcd;
    int getPadding(Screen *screen, int i);
    void pleaseWait();
  public:
    UI(LiquidCrystal *lcd);
    void printScreen(Screen *screen, bool inChange = false);
    void calibrating();
    void saving();
};

#endif
