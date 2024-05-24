#ifndef Magnet_H
#define Magnet_H

#include <Arduino.h>
#include <MPU9250_WE.h>
#include <Wire.h>
#define MPU9250_ADDR 0x68


struct MagData {
  xyzFloat avg;
  int avgMag;
};

//---------Function declaration-----------
void MagScanSetup();

MagData MagScan(int num);
//----------------------------------------

#endif