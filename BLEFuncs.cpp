#include "BLEFuncs.h"


//---------Variable declaration-----------
//Max address number
const int MaxAddress = 5;
//BLE addresses to scan for 
const char* BLEAddress[] = {"48:27:E2:84:3C:ED", "48:27:E2:84:3D:2D", "48:27:E2:84:3D:41", "48:27:E2:84:3D:01", "48:27:E2:84:3C:F9"};
//----------------------------------------

//---------Setup Function-----------------
void BLEScanSetup(){

  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }
  Serial.println("Bluetooth® Low Energy Central scan initialized");
  BLE.begin();
}
//----------------------------------------


//---------Scan Function------------------
std::vector<BLEInfo> BLEScan(int num){
  Serial.println("Scanning for BLE addresses...");
  std::vector<BLEInfo> scan;
  BLEInfo BLEs[MaxAddress];

  for(int i = 0; i < MaxAddress; ++i){
    int sum = 0;
    int amount = 0;
    for(int n = 0; n < num; ++n){

    unsigned long startTime = millis();
    unsigned long elapsedTime;
    bool loop = true;

    BLE.scanForAddress(BLEAddress[i]);
    delay(25);
    BLEDevice peripheral = BLE.available();
    
    while(loop){
      elapsedTime = millis() - startTime;
      if(elapsedTime <= 400){
        if(peripheral){
          sum += peripheral.rssi();
          amount += 1;
          BLE.stopScan();
          loop = false;
        }else{
          BLE.scanForAddress(BLEAddress[i]);
          delay(25);
          peripheral = BLE.available();
        }
      }else{
        loop = false;
      }
    }
  }
  if(amount == 0){
    amount = 1;
  }
   BLEs[i].RSSI = sum / amount;
    BLEs[i].Address = BLEAddress[i];
    scan.push_back(BLEs[i]);
  }
  return scan;
}
//----------------------------------------
