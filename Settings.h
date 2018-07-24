#ifndef SETTINGS_H
#define SETTINGS_H
#include "Arduino.h"
#include "EEPROM.h"
#define CONFIG_START 32
#define CONFIG_VERSION "4.20"

#define KEY_MENU 8
#define KEY_UP 9
#define KEY_DOWN 7

#define BACKLIGHT_PIN 11
#define CONTRAST_PIN 6
#define LCD_RS 12
#define LCD_EN 10
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

class Settings {
  private:
    struct Storage {
      char ver[7] = CONFIG_VERSION;
      float vStep = 0.025;
      float vMax = 0.0;
      float minPow = -65.0;
      int attenuator = 30;
      byte backlight = 80;
      byte contrast = 80;
    } _settings;
  public:
    Settings();
    bool load();
    void save();
    float vStep();
    float vMax();
    float minPow();
    int attenuator();
    byte backlight();
    byte contrast();
    void setVStep(float vStep);
    void setVMax(float vMax);
    void setMinPow(float minPow);
    void setAttenuator(int attenuator);
    void setBacklight(byte brightness);
    void setContrast(byte contrast);
};

#endif

