#include "UI.h"
#include "Meter.h"

UI::UI(LiquidCrystal *lcd) {
  _lcd = lcd;
  _lcd->begin(16, 2);
  _lcd->createChar(1, _dBlogo);
}

int UI::getPadding(Screen *screen, int i) {
  int dec = screen->decimalPoint(i);
  float val = screen->value(i);
  int x = 1;
  int y = screen->unit(i).length();
  x += y;
  if (dec > 0) x += (dec + 1);
  if (val < 0) x++;
  val = abs(val);
  if (val > 1000) x++;
  if (val > 100) x++;
  if (val > 10) x++;
  if (x > 8) x = 8;
  return x;
}

void UI::printScreen(Screen *screen, bool inChange) {
  _lcd->clear();
  _lcd->print(screen->title());
  switch (screen->params()) {
    case 3:
      if (screen->unit(2) == "dBm") _lcd->setCursor(16-getPadding(screen, 2),0);
      else _lcd->setCursor(14-getPadding(screen, 2),0);
      _lcd->print(screen->value(2), screen->decimalPoint(2));
      _lcd->setCursor(16-screen->unit(2).length(),0);
      _lcd->print(screen->unit(2));
    case 2:
      _lcd->setCursor(16-getPadding(screen, 1),1);
      _lcd->print(screen->value(1), screen->decimalPoint(1));
      _lcd->setCursor(16-screen->unit(1).length(),1);
      _lcd->print(screen->unit(1));
    case 1: 
      _lcd->setCursor(0,1);
      if (inChange) _lcd->print((char)0x7E);  
      _lcd->print(screen->value(0), screen->decimalPoint(0));
      _lcd->print(screen->unit(0));
  }
  
}

void UI::calibrating() {
  _lcd->clear();
  _lcd->setCursor(0,0);
  _lcd->print("Auto-Calibrating");
  pleaseWait();
}

void UI::saving() {
  _lcd->clear();
  _lcd->setCursor(4,0);
  _lcd->print("Saving");
  pleaseWait();
}

void UI::pleaseWait() {
  _lcd->setCursor(1,1);
  _lcd->print("Please Wait");
  delay(250);
  _lcd->print(".");
  delay(250);
  _lcd->print(".");
  delay(250);
  _lcd->print(".");
  delay(250);
}


