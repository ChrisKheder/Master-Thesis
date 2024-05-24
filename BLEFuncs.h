#ifndef BLEFuncs_H
#define BLEFuncs_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <string>
#include <vector>

struct BLEInfo {
  int RSSI;
  String Address;
};

//---------Function declaration------------
void BLEScanSetup();

std::vector<BLEInfo> BLEScan(int num);
//----------------------------------------

#endif
