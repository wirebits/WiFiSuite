/*
 * Beacon Header File
 * Author - WireBits
 */

#ifndef BEACON_H
#define BEACON_H

#include <Arduino.h>

extern int cloneCount;
extern bool randomMode;
extern String baseSSID;
extern bool wpa2Enabled;
extern String ssidCharset;
extern bool attackRunning;
extern int randomSSIDLength;

void sendBeacon(char* ssid, bool wpa2);
void getMacFromList(int index, uint8_t outMac[6]);
void sendCustomBeacons(char* base_ssid, int count, bool wpa2);

#endif