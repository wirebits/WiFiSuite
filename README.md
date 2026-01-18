# WiFiSuite
A collection of tools for 2.4GHz wifi networks on ESP8266 chipset.

# Key Features
- Simple Setup.
- Control via Seial Console.
- Easy to manage.
- Deauthentication Attack
- Beacon Flood Attack

# Supported Board
- It supports NodeMCU ESP8266 Boards only.
- It supports `2.4GHz` frequency only.
- If possible, use those NodeMCU ESP8266 boards which contain `CP2102` driver chipset.

# Setup
1. Download Arduino IDE from [here](https://www.arduino.cc/en/software) according to your Operating System.
2. Install it.
3. Go to `File` → `Preferences` → `Additional Boards Manager URLs`.
4. Paste the following link :
   
   ```
   https://raw.githubusercontent.com/SpacehuhnTech/arduino/main/package_spacehuhn_index.json
   ```
6. Click on `OK`.
7. Go to `Tools` → `Board` → `Board Manager`.
8. Wait for sometimes and search `deauther` by `Spacehuhn Technologies`.
9. Simply install it.
10. Wait for sometime and after that it is installed.
11. Restart the Arduino IDE.
12. Done!

# Install
## Via Arduino IDE
1. Download or Clone the Repository.
2. Open the folder and just double click on `WiFiSuite.ino` file.
3. It opens in Arduino IDE.
4. Compile the code.
5. Select the correct board from the `Tools` → `Board` → `Deauther ESP8266 Boards`.
   - It is `NodeMCU`.
6. Select the correct port number of that board.
7. Upload the code.
8. Done!
## Via ESP8266 Flasher
1. Download the NodeMCU ESP8266 Flasher from [here](https://github.com/nodemcu/nodemcu-flasher) according to your operating system.
2. Download the `.bin` file from [here](https://github.com/wirebits/CLI-Deauther/releases/download/v1.0.0/CLI-Deauther.bin).
3. Open NodeMCU ESP8266 Flasher.
4. Select the correct port number of that board.
5. Click on `Advanced` Tab.
6. Click on `Restore Default` button.
7. Click on `Config` Tab.
   - It show `INTERNAL://NODEMCU`.
8. Click on ![image](https://github.com/user-attachments/assets/1540d7e8-514a-4e60-a29d-3019699868df) in front of `INTERNAL://NODEMCU`.
9. Select the `CLI-Deauther.bin` file.
10. Click on `Operation` Tab.
11. Click on `Flash(F)` button.
12. Wait for sometimes and when completed, press `RST` button.

# Using Serial Console
1. Open Serial Console by click [here](https://wirebits.github.io/SerialConsole/).
2. Select baud rate `115200`.
3. Click on `Connect` button.
4. When it shows `Connected! Go On!` only then your ESP8266 board is ready otherwise repeat steps 2 and 3.
5. Type `help` to get list of helps.
