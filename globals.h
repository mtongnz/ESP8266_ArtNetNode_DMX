/*

ESP8266_ArtNetNode_DMX - globals.h
Copyright (c) 2015, Matthew Tong    
https://github.com/mtongnz/ESP8266_ArtNetNode_DMX

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see http://www.gnu.org/licenses/

*/


/////////  Global Variables.  //////////

WiFiUDP eUDP;
ESP8266WebServer webServer(80);

#define ARTNET_PORT 0x1936
#define ARTNET_BUFFER_MAX 600
#define ARTNET_REPLY_SIZE 239
#define ARTNET_ADDRESS_OFFSET 18
#define ARTNET_ARTDMX 0x5000
#define ARTNET_ARTPOLL 0x2000

enum dmxStates {DMX_STOPPED, DMX_STARTED};
bool dmxStateA = DMX_STOPPED;
bool dmxStateB = DMX_STOPPED;
bool newDataA = 0;
bool newDataB = 0;

unsigned char packetBuffer[ARTNET_BUFFER_MAX];
uint16_t hotSpotDelay = 30;
char nodeName[30] = "artNetNode";
uint8_t artNetUniA = 0;
uint8_t artNetUniB = 1;
uint8_t artNetSub = 0;
char wifiSSID[30] = "defaultSSID";
char wifiPass[30] = "defaultPassword";
bool dhcp = 1;
bool isHotSpot = 0;
bool allowHotSpot = 1;
bool standAlone = 0;
bool restart = 0;

unsigned long lastWebServe = 0;
unsigned long lastFullDMXA = 0;

bool outputScene = 0;
uint16_t outputSceneNum = 0;

uint8_t MAC_array[6];
String MAC_address;
IPAddress ap_ip(2, 0, 0, 10);
IPAddress ip(2, 0, 0, 10);
IPAddress subnet(255, 0, 0, 0);
IPAddress broadcast_ip(ip[0], 255, 255, 255);

