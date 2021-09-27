### Sample projects for Arduino based on ESP8266 board

#### Pre-requests Arduino IDE

* Open Arduino IDE and go to `File > Preferences`
* In the option dialog find field `Additional Boards Manager URLs` and type `http://arduino.esp8266.com/stable/package_esp8266com_index.json`

![ESP8266_Settings](./images/ArduinoESP8266Settings.png)

* Next step - open `Tools > Board (...) > Board Manager...` dialog and install ESP8266 support

![ESP8266_Support](./images/ESP8266Community.png)

* After that connect board to your PC and change its type to `Generic ESP8266 Module`

![GenericModuleESP8266](./images/ESP8266Board.png)

* After that install libraries using `Sketch > Include library > Manage libraries...`
 * `Blynk`
 
 ![Blynk_Lib](./images/BlynkAddLib.png)

* WiFiManager

![WiFiManager](./images/WifiManager.png)
