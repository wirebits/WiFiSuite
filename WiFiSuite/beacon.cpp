/*
 * Beacon CPP File
 * Author - WireBits
 */

#include "beacon.h"
#include <ESP8266WiFi.h>

extern "C" {
  #include "user_interface.h"
}

int cloneCount = 0;
String baseSSID = "";
int macIndexCounter = 0;
bool wpa2Enabled = true;
bool randomMode = false;
const int macCount = 100;
int randomSSIDLength = 8;
bool attackRunning = false;
const uint8_t macStartLastByte = 0x11;
const uint8_t macBaseFirst5[5] = {0x02, 0x1A, 0x3C, 0x4F, 0x9B};
String ssidCharset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

void getMacFromList(int index, uint8_t outMac[6]) {
  memcpy(outMac, macBaseFirst5, 5);
  uint8_t last = macStartLastByte + (index % macCount);
  outMac[5] = last;
}

void sendCustomBeacons(char* base_ssid, int count, bool wpa2) {
  if (randomMode) {
    for (int i = 0; i < count; i++) {
      String ssid = "";
      int charsetLen = ssidCharset.length();
      for (int p = 0; p < randomSSIDLength; p++) {
        ssid += ssidCharset.charAt(random(charsetLen));
      }
      char buffer_ssid[33];
      ssid.toCharArray(buffer_ssid, 33);
      sendBeacon(buffer_ssid, wpa2);
      delay(1);
    }
    return;
  }
  int base_ssid_lenght = strlen(base_ssid);
  int i = 0;
  for (int j = 0; j < 32 - base_ssid_lenght; j++) {
    for (int k = 0; k < pow(2, j); k++) {
      int sp_ch = k;
      String ssid = base_ssid;
      for (int repeat = 0; repeat < j; repeat++) {
        ssid += (sp_ch % 2 == 1) ? " " : "\t";
        sp_ch /= 2;
      }
      char buffer_ssid[33];
      ssid.toCharArray(buffer_ssid, 33);
      sendBeacon(buffer_ssid, wpa2);
      delay(1);
      if (i++ >= count) {
        return;
      }
    }
  }
}

void sendBeacon(char* ssid, bool wpa2) {
  byte channels[] = {1, 6, 11};
  byte channel = channels[random(0, 3)];
  wifi_set_channel(channel);
  uint8_t beaconPacket[256] = {
    0x80, 0x00,
    0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x00, 0x00,
    0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00,
    0xe8, 0x03,
    0x00, 0x00,
    0x00, 0x20,
  };
  int beacon_ssid_lenght = strlen(ssid);
  if (beacon_ssid_lenght > 32) beacon_ssid_lenght = 32;
  beaconPacket[37] = beacon_ssid_lenght;
  for (int i = 0; i < beacon_ssid_lenght; i++) {
    beaconPacket[38 + i] = ssid[i];
  }
  int beacon_packet_position = 38 + beacon_ssid_lenght;
  uint8_t ratesAndChannel[] = {
    0x01, 0x08,
    0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c,
    0x03, 0x01,
    0x00
  };
  ratesAndChannel[10] = channel;
  memcpy(&beaconPacket[beacon_packet_position], ratesAndChannel, sizeof(ratesAndChannel));
  beacon_packet_position += sizeof(ratesAndChannel);
  if (wpa2) {
    beaconPacket[34] |= 0x10;
    uint8_t rsnInfo[] = {
      0x30, 0x18,
      0x01, 0x00,
      0x00, 0x0f, 0xac, 0x02,
      0x02, 0x00,
      0x00, 0x0f, 0xac, 0x04,
      0x00, 0x0f, 0xac, 0x04,
      0x01, 0x00,
      0x00, 0x0f, 0xac, 0x02,
      0x00, 0x00
    };
    memcpy(&beaconPacket[beacon_packet_position], rsnInfo, sizeof(rsnInfo));
    beacon_packet_position += sizeof(rsnInfo);
  } else {
    beaconPacket[34] &= ~0x10;
  }
  uint8_t useMac[6];
  getMacFromList(macIndexCounter, useMac);
  macIndexCounter = (macIndexCounter + 1) % macCount;
  for (int i = 0; i < 6; i++) {
    beaconPacket[10 + i] = useMac[i];
    beaconPacket[16 + i] = useMac[i];
  }
  wifi_send_pkt_freedom(beaconPacket, beacon_packet_position, 0);
}