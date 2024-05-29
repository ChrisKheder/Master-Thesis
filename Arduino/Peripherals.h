#ifndef Peripherals_H
#define Peripherals_H

#include <Arduino.h>
#include<utility>
#include "BLEFuncs.h"
#include "WIFIFuncs.h"
#include "Magnet.h"
#include <sstream>


//---------Function declaration------------
void PeripheralSetup();

std::vector<int> ScanButton(int num);

int IncreButton(int Val, int max);
int DecreButton(int Val, int max);

bool PhaseSelector();

int BatStatus();

std::pair<int, int> NavigateGrid(int row, int col);
int GetRP(int row, int col);

void ExtractMeasurement(const char* address, int val, std::vector<int>& measurement );
void InsertMeasurement(int RSSI[][10], int row, const std::vector<int>& measurement);
std::string measurementToString(const std::vector<int>& measurement);
String MQTTString(int rp, const std::vector<int>& measurement);
void printMatrix(int RSSI[][10], int numRows);
//----------------------------------------

#endif