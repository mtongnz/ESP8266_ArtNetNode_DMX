/*

ESP8266_ArtNetNode_DMX - eeprom.ino
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


/* saveSettings()
 *  Save our variables into EEPROM
 *  Writes OK to 500 & 501 - use this to verify a write
 */
bool saveSettings() {
  #ifdef VERBOSE
    Serial.print("Saving Settings... ");
  #endif

  // Wipe our OK and check
  EEPROM.write(500, '\0');
  EEPROM.write(501, '\0');
  EEPROM.commit();
  if(EEPROM.read(500) != '\0'|| EEPROM.read(501) != '\0') {
    #ifdef VERBOSE
      Serial.println("fail!");
    #endif
    
    return false;
  }
  
  int x;
  delay(100);
  
  for(x = 0; x < 30 && nodeName[x] != '\0'; x++)
    EEPROM.write(x, nodeName[x]);
  EEPROM.write(x, '\0');
  EEPROM.write(30, artNetUniA);
  EEPROM.write(130, artNetUniB);
  EEPROM.write(31, artNetSub);
  
  for(x = 0; x < 30 && wifiSSID[x] != '\0'; x++)
    EEPROM.write(32 + x, wifiSSID[x]);
  EEPROM.write(32 + x, '\0');
  for(x = 0; x < 30 && wifiPass[x] != '\0'; x++)
    EEPROM.write(62 + x, wifiPass[x]);
  EEPROM.write(62 + x, '\0');
  
  delay(100);
  
  EEPROM.write(92, dhcp);
  EEPROM.write(93, ip[0]);
  EEPROM.write(94, ip[1]);
  EEPROM.write(95, ip[2]);
  EEPROM.write(96, ip[3]);
  EEPROM.write(97, broadcast_ip[0]);
  EEPROM.write(98, broadcast_ip[1]);
  EEPROM.write(99, broadcast_ip[2]);
  EEPROM.write(100, broadcast_ip[3]);
  EEPROM.write(101, subnet[0]);
  EEPROM.write(102, subnet[1]);
  EEPROM.write(103, subnet[2]);
  EEPROM.write(104, subnet[3]);
  EEPROM.write(106, hotSpotDelay & 0xFF);
  EEPROM.write(107, (hotSpotDelay >> 8) & 0xFF);
  EEPROM.write(108, standAlone);

  EEPROM.write(500, 'O');
  EEPROM.write(501, 'K');

  delay(100);
  
  EEPROM.commit();
  
  delay (100);

  // Verify our OK was written & return false if not
  if(EEPROM.read(500) != 'O' || EEPROM.read(501) != 'K') {
    #ifdef VERBOSE
      Serial.println("fail!");
    #endif
    
    return false;
  }
  
  #ifdef VERBOSE
    Serial.println("success!");
  #endif

  // Return true if all went well
  return true;
}



/* loadSettings()
 *  Load our settings from EEPROM to our variables.
 */
bool loadSettings() {
  #ifdef VERBOSE
    Serial.print("Loading Settings... ");
  #endif
  
  int x;

  // If no previous settings, start hotspot
  if(EEPROM.read(502) != 'R') {
    EEPROM.write(502, 'R');
    
    return false;
  }
  
  hotSpotDelay = (EEPROM.read(106) & 0xFF) + ((EEPROM.read(107) << 8) & 0xFF00);
  standAlone = EEPROM.read(108);
  
  // Check if we have previous saves.  If not, return false
  if(EEPROM.read(500) != 'O' || EEPROM.read(501) != 'K') {
    #ifdef VERBOSE
      Serial.println("No previous saves.");
    #endif
    
    return false;
  }

  for(x = 0; x < 30; x++) {
    nodeName[x] = EEPROM.read(x);
    if (nodeName[x] == '\0')
      break;
  }
  nodeName[x] = '\0';
  
  artNetUniA = EEPROM.read(30);
  artNetUniB = EEPROM.read(130);
  artNetSub = EEPROM.read(31);
  
  for(x = 0; x < 30; x++) {
    wifiSSID[x] = EEPROM.read(32 + x);
    if (wifiSSID[x] == '\0')
      break;
  }
  wifiSSID[x] = '\0';
  for(x = 0; x < 30; x++) {
    wifiPass[x] = EEPROM.read(62 + x);
    if (wifiPass[x] == '\0')
      break;
  }
  wifiPass[x] = '\0';
  
  dhcp = EEPROM.read(92);

  // Only store IP if we're not using DHCP
  if (dhcp != 1) {
    ip[0] = EEPROM.read(93);
    ip[1] = EEPROM.read(94);
    ip[2] = EEPROM.read(95);
    ip[3] = EEPROM.read(96);
    broadcast_ip[0] = EEPROM.read(97);
    broadcast_ip[1] = EEPROM.read(98);
    broadcast_ip[2] = EEPROM.read(99);
    broadcast_ip[3] = EEPROM.read(100);
    subnet[0] = EEPROM.read(101);
    subnet[1] = EEPROM.read(102);
    subnet[2] = EEPROM.read(103);
    subnet[3] = EEPROM.read(104);
  }
  
  #ifdef VERBOSE
    Serial.println("success!");
  #endif

  artNetUniA = artNetUniA;
  artNetUniB = artNetUniB;

  // Return
  return true;
}

