#ifndef Display_H
#define Display_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "Peripherals.h"
#include "MQTTFuncs.h"



//---------Function declaration-----------
void DisplaySetup();
void DeviceInit();

void StartScreen();
void MainScreen();
void OfflineScreen();
void OnlineScreen();
void ScanScreen();
void ArrayScreen(int rp);

void CenterClear();
void SmallClear(int X, int Y);

void RP();
void Battery(int lvl);
void Phase();

void PhasePointer();
void SelectedPhase(int Phase);


void RPPointer();
//----------------------------------------

#endif
