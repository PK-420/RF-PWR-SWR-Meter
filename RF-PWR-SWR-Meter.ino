#include "Settings.h"
#include <LiquidCrystal.h>
#include "Meter.h"
#include "UI.h"
#include "Screen.h"

// #define DEBUG

/*
  RF Power Meter / SWR Meter
*/

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Settings settings;
Meter meter(A7);
UI ui(&lcd);

// Input & Buttons
#define INPUT_QTY 3
const int inputPins[INPUT_QTY] = {KEY_UP, KEY_MENU, KEY_DOWN};
int lastInputState[INPUT_QTY] = {LOW, LOW, LOW};
int inputState[INPUT_QTY];
bool inputFlags[INPUT_QTY] = {LOW, LOW, LOW};
bool inputLongFlags[INPUT_QTY] = {LOW, LOW, LOW};
bool longPressActive[INPUT_QTY] = {false};
long buttonTimer[INPUT_QTY] = {0};
long longPressTime = 500;
long lastDebounceTime[INPUT_QTY] = {0, 0, 0};
long debounceDelay = 10;

//LCD Menu Logic

enum screenOrder {
  CALIB,
  SWR,
  PWR,
  ATTEN,
  V_MAX,
  VSTEP,
  MIN_P,
  LIGHT,
  CONTR,
  SCREEN_QTY,
};
#define MENU_FIRST ATTEN
int currentScreen = PWR;
int lastScreen = MENU_FIRST;
bool inMenu = false;
bool inChange = false;
bool measuring = false;
Screen* screen = new Screen[SCREEN_QTY];

void initScreens() { // Screen Preparation
  screen[SWR] = Screen("SWR", "dBm", "dBm", " : 1", 0, 0, 9000.11);
  screen[SWR].setDecimalPoint(1, 0);
  screen[SWR].setDecimalPoint(1, 1);
  screen[SWR].setDecimalPoint(4, 2);
  screen[PWR] = Screen("Estimated Power", "dBm", "mW");
  screen[PWR].setDecimalPoint(1);
  screen[ATTEN] = Screen("Attenuator", "dB", settings.attenuator());
  screen[ATTEN].setLimits(0,60);
  screen[ATTEN].setStep(10);
  screen[V_MAX] = Screen("Calibration", "vMax", settings.vMax());
  screen[V_MAX].setDecimalPoint(3);
  screen[VSTEP] = Screen("vStep", "V/dB", settings.vStep());
  screen[VSTEP].setLimits(.015, .042);
  screen[VSTEP].setDecimalPoint(3);
  screen[VSTEP].setStep(0.001);
  screen[MIN_P] = Screen("Offset", "dBm", settings.minPow());
  screen[MIN_P].setLimits(-70, -50);
  screen[MIN_P].setDecimalPoint(1);
  screen[MIN_P].setStep(0.1);
  screen[LIGHT] = Screen("Backlight", "%", map(settings.backlight(), 0, 200, 0, 100));
  screen[LIGHT].setLimits(0,100);
  screen[LIGHT].setStep(1);
  screen[CONTR] = Screen("Contrast", "%", map(settings.contrast(), 110, 10, 0, 100));
  screen[CONTR].setLimits(0,100);
  screen[CONTR].setStep(1);
  screen[CALIB] = Screen("OK?  << N | Y >>", " vMax");
  screen[CALIB].setDecimalPoint(9);
}

// TestThings

double previous = 0;


void setup() {
  pinMode(KEY_MENU, INPUT_PULLUP);
  pinMode(KEY_UP, INPUT_PULLUP);
  pinMode(KEY_DOWN, INPUT_PULLUP);

  if (settings.vMax() == 0) {
    vMaxCalibration();
  }
#ifdef DEBUG
  Serial.begin(115200);
  debug();
#endif
// Set Base Values for Screens
  initScreens();
}

void loop() {
  setInputFlags();
  resolveInputFlags();
  if ((millis() % 500 == 0) && (
    (currentScreen == PWR) || (currentScreen == CALIB) || ((currentScreen == SWR) && (measuring))
    )){
      double voltage = meter.voltage();
      float dBmPower = v2dBm(voltage) + settings.attenuator();
      double mWPower = dBm2mW(dBmPower);
      screen[CALIB].changeValue(voltage);
      screen[PWR].changeValue(dBmPower);
      int m = 0;
      while (mWPower < 1) {
        mWPower *= 1000;
        m++;
      }
      switch (m) {
        case 0: 
          screen[PWR].changeUnit("mW", 1);
          break;
        case 1:
          screen[PWR].changeUnit((String)(char)0xE4+"W", 1);
          break;
        case 2:
          screen[PWR].changeUnit("nW", 1);
          break;  
        case 3:
          screen[PWR].changeUnit("pW", 1);
          break; 
        case 4:
          screen[PWR].changeUnit("fW", 1);
          break; 
        case 5:
          screen[PWR].changeUnit("aW", 1);
          break; 
      }
      screen[PWR].changeValue(mWPower, 1);
      if (measuring) {
        screen[SWR].changeValue(dBmPower, 2);
      }
      ui.printScreen(&screen[currentScreen]);
  }  
}

void setInputFlags() {
  for (int i = 0; i < INPUT_QTY; i++) {
    int reading = digitalRead(inputPins[i]);
    if (reading != lastInputState[i]) {
      lastDebounceTime[i] = millis();
    }
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading == LOW) { // Button Down
        if (inputState[i] == LOW) {
          inputState[i] = HIGH;
          buttonTimer[i] = millis();
        }
        if ((millis() - buttonTimer[i] > longPressTime) && (!longPressActive[i])) {
          //Long Press
          longPressActive[i] = true;
          inputLongFlags[i] = HIGH;
        }
      } else { // Button Released
        if (inputState[i] == HIGH) {
          if (longPressActive[i] == true) {
            // Long Press Released
            longPressActive[i] = false;
          } else {
            // Short Press Released
            inputFlags[i] = HIGH;
          }
          inputState[i] = LOW;
        }
      }
    }   
    lastInputState[i] = reading;
  }
}

void resolveInputFlags() {
  for (int i = 0; i < INPUT_QTY; i++) {
    if ((inputFlags[i] == HIGH) || (inputLongFlags[i] == HIGH)){
      if (inputLongFlags[i] == HIGH) inputLongAction(i);
      else inputAction(i);
      inputFlags[i] = LOW;
      inputLongFlags[i] = LOW;
      // Print New Screen (menu...)
      ui.printScreen(&screen[currentScreen], inChange);
    }
  }
}

void inputAction(int input) {
  switch (input) {
    case 0: // Key <<<
      if ((currentScreen == SWR) && (measuring)) {
        screen[SWR].changeValue(screen[SWR].value(2), 0);
      } else if (currentScreen == CALIB) {
        currentScreen = V_MAX;
      } else if (inChange) {
        //Value Change
        screen[currentScreen].decrement();
        updateValues(currentScreen);
      } else if (inMenu) {
        // Menu
        if (currentScreen > MENU_FIRST) currentScreen--;
        else currentScreen = SCREEN_QTY - 1;
      } else {
        // Normal
        if (currentScreen > CALIB + 1) currentScreen--;
        else currentScreen = MENU_FIRST - 1;
      }
      break;
    case 1: // Key Menu
      if (currentScreen == CALIB) currentScreen = V_MAX;
      else if ((currentScreen == SWR) && (!measuring)) {
        measuring = true;
        screen[SWR].changeTitle("Measure");
        screen[SWR].changeValue(-99.9,2);
        screen[SWR].changeUnit("dBm", 2);
      } else if ((currentScreen == SWR) && (measuring)) {
        measuring = false;
        screen[SWR].changeTitle("SWR");
        screen[SWR].changeUnit(" : 1", 2);
        double ref = dBm2mW(screen[SWR].value());
        double fwd = dBm2mW(screen[SWR].value(1));
        if (ref > fwd) {
          double fuck = fwd;
          fwd = ref;
          ref = fuck;
        }
        screen[SWR].changeValue(swr(fwd, ref),2);
      }
      else if (inChange) { // Exit Change
        inChange = false;
        if (previous != screen[currentScreen].value()) saveAll();
      } else if (inMenu) { // Exit Menu
        inMenu = false;
        int tScreen = currentScreen;
        currentScreen = lastScreen;
        lastScreen = tScreen;
      } // else do nothing
      break;
    case 2: // Key >>>
      if ((currentScreen == SWR) && (measuring)) {
        screen[SWR].changeValue(screen[SWR].value(2), 1);
      } else if (currentScreen == CALIB) break;
      else if (inChange) {
        //Value Change
        screen[currentScreen].increment();
        updateValues(currentScreen);
      } else if (inMenu) {
        // Menu
        if (currentScreen < SCREEN_QTY - 1) currentScreen++;
        else currentScreen = MENU_FIRST;
      } else {
        // Normal
        if (currentScreen < MENU_FIRST - 1) currentScreen++;
        else currentScreen = CALIB + 1;
      }
      break;
  }
}

void inputLongAction(int input) {
  switch (input) {
    case 0: // Key <<<
      switch (currentScreen) {
        case CALIB:
          currentScreen = V_MAX;
          break;
        case SWR:
        case PWR:
        case ATTEN:
        case V_MAX:
        case VSTEP:
        case MIN_P:
        case LIGHT:
        case CONTR:
        break;
      }
      break;
    case 1: // Key Menu
      if (!measuring) {
        if (!inMenu) {
          inMenu = true;
          int tScreen = currentScreen;
          currentScreen = lastScreen;
          lastScreen = tScreen;
        } else if (!inChange){
          if (currentScreen == V_MAX) currentScreen = CALIB;
          else {
            inChange = true;
            previous = screen[currentScreen].value();
          }
        }
      }
      break;
    case 2: // Key >>>
        switch(currentScreen) {
        case CALIB:
          vMaxCalibration();
          screen[V_MAX].changeValue(settings.vMax());
          currentScreen = V_MAX;
        case SWR:
        case PWR:
        case ATTEN:
        case V_MAX:
        case VSTEP:
        case MIN_P:
        case LIGHT:
        case CONTR:
        break;
      }
      break;
  }
}

void updateValues(int screenID) {
  switch (screenID) {
    case PWR:
      break;
    case ATTEN: settings.setAttenuator((int)screen[screenID].value());
      break;
    case V_MAX: settings.setVMax(screen[screenID].value());
      break;
    case VSTEP: settings.setVStep(screen[screenID].value());
      break;
    case MIN_P: settings.setMinPow(screen[screenID].value());
      break;
    case LIGHT: settings.setBacklight((byte)map(screen[screenID].value(), 0, 100, 0, 200));
      break;
    case CONTR: settings.setContrast((byte)map(screen[screenID].value(), 0, 100, 110, 10));
      break;
  }
}

void saveAll() {
  ui.saving();
  settings.save();
}

void vMaxCalibration() {
  ui.calibrating();
  
  settings.setVMax(meter.vAvg(10));
  settings.save();
}

float v2dBm(double voltage) {
  return ((settings.vMax() - voltage) / settings.vStep()) + settings.minPow();
}

double dBm2mW(float dBm) {
  return pow(10.0, (dBm / 10));
}

double swr(double fwd, double ref) {
  double root = sqrt(ref / fwd);
  return (1 + root) / (1 - root);
}

void debug() {
  double volts = meter.voltage();
  Serial.print("Config Version: ");
  Serial.println(CONFIG_VERSION);
  Serial.print("Vcc: ");
  Serial.print(meter.vcc(), 4);
  Serial.print("V\tvMax: ");
  Serial.print(settings.vMax(), 4);
  Serial.print("V\t\tMeasured: ");
  Serial.print(volts, 4);
  Serial.print("V\tvStep: ");
  Serial.print(settings.vStep(), 4);
  Serial.println("V/dB");
  Serial.print("Backlight: ");
  Serial.print(settings.backlight());
  Serial.print("\tOffset: ");
  Serial.print(settings.minPow());
  Serial.print("dBm\tPower: ");
  Serial.print(v2dBm(volts));
  Serial.print("dBm\tAttenuator: ");
  Serial.print(settings.attenuator());
  Serial.println("dB");
  Serial.print("Contrast: ");
  Serial.print(settings.contrast());
}

