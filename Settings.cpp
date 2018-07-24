#include "Settings.h"

Settings::Settings() {
  if (!(load())) {
    save();
  }
  analogWrite(BACKLIGHT_PIN, _settings.backlight);
  analogWrite(CONTRAST_PIN, _settings.contrast);
}

bool Settings::load() {
  for (unsigned int i = 0; i < sizeof(_settings.ver); i++) if ( !(EEPROM.read(CONFIG_START + i) == CONFIG_VERSION[i]) ) return false;
  for (unsigned int j = 0; j < sizeof(_settings); j++) *((char*)&_settings + j) = EEPROM.read(CONFIG_START + j);
  
  return true;
}

void Settings::save() {
  for (unsigned int i = 0; i < sizeof(_settings); i++) EEPROM.write(CONFIG_START + i, *((char*)&_settings + i));
}

float Settings::vStep() {
  return _settings.vStep;
}

float Settings::vMax() {
  return _settings.vMax;
}

float Settings::minPow() {
  return _settings.minPow;
}

int Settings::attenuator() {
  return _settings.attenuator;
}

byte Settings::backlight() {
  return _settings.backlight;
}

byte Settings::contrast() {
  return _settings.contrast;
}

void Settings::setVStep(float vStep) {
  _settings.vStep = vStep;
}

void Settings::setVMax(float vMax) {
  _settings.vMax = vMax;
}

void Settings::setMinPow(float minPow) {
  _settings.minPow = minPow;
}

void Settings::setAttenuator(int attenuator) {
  _settings.attenuator = attenuator;
}

void Settings::setBacklight(byte brightness) {
  analogWrite(BACKLIGHT_PIN, brightness);
  _settings.backlight = brightness;
}

void Settings::setContrast(byte contrast) {
  analogWrite(CONTRAST_PIN, contrast);
  _settings.contrast = contrast;
}

