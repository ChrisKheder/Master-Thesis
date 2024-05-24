#ifndef MQTTFuncs_H
#define MQTTFuncs_H

#include <Arduino.h>
#include "WIFIFuncs.h"
#include <MQTT.h>


//---------Function declaration-----------
void MQTTSetup();
void Connect();
void Pub(const String message);
//----------------------------------------
#endif