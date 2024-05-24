#ifndef WIFIFuncs_H
#define WIFIFuncs_H

#include <Arduino.h>
#include <WiFi.h>
#include <string>
#include <vector>
#include <tuple>
#include <map>

struct WiFiInfo {
  int RSSI;
  String MAC;
};

//---------Function declaration-----------
void WiFiScanSetup();

std::vector<WiFiInfo> SingleWiFiScan();

std::vector<WiFiInfo> WiFiScan(int num);
//----------------------------------------
#endif