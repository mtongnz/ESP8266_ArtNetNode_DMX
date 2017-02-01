# ESP8266_ArtNetNode_DMX
ESP8266 based WiFi ArtNet to DMX - Supports 2 full universes

The new and improved ArtNetNode_V2 is currently in beta here: https://github.com/mtongnz/ESP8266_ArtNetNode_v2

View my Instructable for the device: http://www.instructables.com/id/ESP8266-Artnet-to-DMX/

Note that I've updated the espDMX library to add new functions required for this update.  Make sure you get the latest version.  The new version should still work fine with any of your existing code.

This code requires my espDMX library available at https://github.com/mtongnz/espDMX and also the Arduino Core file for ESP8266 here: https://github.com/esp8266/Arduino

If you find this project helpful and you're feeling generous, I'd love for you to buy me a beer: https://www.paypal.me/mtongnz

Latest Release: v1.2.0

##CHANGES v1.2.0
 - Uses latest version of espDMX to increase refresh rate and add new features.
 - Ability to store and recall scenes from web browser.
 - Stability improvements.

##CHANGES v1.1.1
Updated open source LwIP: https://github.com/mtongnz/Arduino to allow other espArtnetNodes to connect to the node when in hotspot mode.  

Source code is the same as v1.1.0 but LwIP has IP_FORWARD enabled: https://github.com/mtongnz/Arduino/pull/2

##CHANGES v1.1.0
Use modified open source LwIP: https://github.com/mtongnz/Arduino to allow setting of subnet in softAP mode.
 - softAP uses 2.0.0.1 IP with subnet 255.0.0.0 in DHCP mode
 - softAP allows a static IP and subnet to be set
 - Added: stand alone mode (hotspot only - no wifi)
 - Added: artnet can be received when in stand alone mode
 - Added: setBroadcastAddr() function to calculate the subnet mask
 - Modified web UI: stand alone mode toggle
 - Modified web UI: images (help/view password)
 - Modified web UI: view password toggle
 - Modified web UI: help toggle buttons

Latest Release Candidate: v1.2.0 https://github.com/mtongnz/ESP8266_ArtNetNode_DMX/tree/v1.2.0

##ABOUT
This turns an ESP8266 into a wireless ArtNet Node.  It will connect to most standard wireless networks and allow a 2 full universes of DMX data to be output.
  
Configuration is done via web browser.  Simply connect to the IP of the node.  Note there is no password requirements. It is assumed that the WiFi network is secure.  It is recommended to use authentication and MAC filtering.  The MAC is provided at the top of the config page.
  
If the device is unable to connect to the WiFi (as will happen the first time it's run) it will start a WiFi hot spot after a preset time (default 30 seconds).  The name of the hot spot should be the node's name followed  by a random number.  The password is artNodeXXXX (where XXXX is the number following _ from the SSID).  If no clients  connect to the hotspot within 1 minute, the node will reset and try again.

##FIRMWARE UPDATE
Firmware update is supported by web upload.  Simply goto the devices IP in a browser and upload the new firmware at the bottom of the main config page.  This is much easier than using a serial upload method - especially if the device is in an awkward to get to spot.

##FIRST RUN
On the first run, power on unit and wait for the artNetNode_XXXX WiFi to appear on your laptop/phone... Connect to the network within 1 minute.  Point your browser to 2.0.0.1 and enter the settings desired.  Click save and reset button and it should now connect to your WiFi.

##CHANGE SETTINGS LATER
If the device connects to your WiFi successfully, simply head to it's IP in your browser and do as above.  Note that you can change ArtNet settings and they will take effect without a reset.  If the node fails to connect to your WiFi, it will start a hot spot after the preset time.  You can also use stand alone mode if you wish to send artnet to the device directly (without an AP).

##GENERAL USE
Once you have the device connecting correctly and the ArtNet settings done, try sending some data from your console. I reccommend sending directly to each device (rather than using ArtNet broadcast) as this will reduce network load. If you have heaps of devices (40+) connected to the same ArtNet Sub & Uni, then it's probably better to use broadcast - but I don't see why anyone would do this.

If the device is working correctly, I would recommend setting the hot spot timeout to a higher number.

##WIRING
I wont supply pin specific wiring as there are many different versions of the ESP8266.
Note that a clean 3.3V supply with 300mA or more is highly recommended for stable operation.

```  
  vcc        3.3V
  rx0        for flashing - must be 3.3V TTL.  Can use a voltage divider if needed
  tx0        DMX-A Output or verbose debug output and also flashing
  tx1        DMX-B output - must use RS485 chip
  gpio1      hold to ground while reseting to enable flashing
  gpio12     LED-A dmx indicator
  gpio13     LED-B dmx indicator
  reset      hold to ground to reset
```

##DEBUG MODE
For debugging, define VERBOSE (uncomment below) to get serial output at 115200 baud.  This will show SSID and passwords in plain text.  It also causes a minor hit to dmx timings, so leave it disabled for normal use.  It will however allow you to see when ArtNet packets and polls are recieved, how many channels in the packet,  any http requests...

##CREDITS
I have used a ton of sites and forums for information, ideas and code snippets.  I would like to thank the amazing communities surrounding the Arduino and ESP8266 devices.

The main libraries/sources of code are:
  - espDMX written by myself, https://github.com/mtongnz/espDMX
  - ESP8266 core for Arduino, https://github.com/esp8266/Arduino
  - ArtNet Library written for Arduino by Chris Staite, yourDream

##FILES
```
  ESP8266_ArtNetNode_DMX.ino  This is the main file. It contains the includes as well as the setup and loop functions.
  artNet.ino      All artNet functions
  eeprom.ino      Functions for saving and loading settings to eeprom
  globals.h       Global variable definitions
  webServer.ino   Web server functions, CSS file, and HTML files.  Remote firmware upload.
  WiFi.ino        All WiFi functions including hotSpot
```
