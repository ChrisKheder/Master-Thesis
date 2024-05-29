#include "Display.h"

void setup() {
  Serial.begin(115200);

  DisplaySetup();
  MagScanSetup();
  WiFiScanSetup();
  BLEScanSetup();
  PeripheralSetup();
  
}

void loop() {
  DeviceInit();
}
