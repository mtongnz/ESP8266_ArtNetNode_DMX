/*

ESP8266_ArtNetNode_DMX - WiFi.ino
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


/* startWiFi()
 *  Connects our WiFi.
 *  If it can't connect and allowHotSpot is set, it will call startHotSpot
 */
void startWifi() {
  #ifdef VERBOSE
    Serial.print("SSID: ");
    Serial.println(wifiSSID);
    Serial.print("Password: ");
    Serial.println(wifiPass);
    Serial.print("Wifi Connecting");
  #endif

  // Connect wifi
  WiFi.begin(wifiSSID, wifiPass);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(nodeName);

  uint16_t x = 0;
  // Wait for WiFi to connect
  while (WiFi.status() != WL_CONNECTED) {
    // delay to yield to esp core functions
    delay(500);
    #ifdef VERBOSE
      Serial.print(".");
    #endif

    // If it hasn't connected within the preset time, start hot spot
    // note the .5 second delay above -> hence hotSpotDelay*2
    if (allowHotSpot == 1 && x++ == hotSpotDelay * 2) {
      #ifdef VERBOSE
        Serial.println(" failed!");
        Serial.println("Starting Hotspot");
      #endif
      
      delay(1000);
      // start our hotspot
      startHotSpot();
    }
  }

  // Get MAC Address
  getMac();

  // If static address, set our wifi to use it 
  if (dhcp != 1) {
    IPAddress gw(ip[0], 0, 0, 1);
    WiFi.config(ip, gw, subnet);

  // If DHCP, get the IPs
  } else {
    ap_ip = WiFi.localIP();
    ip = WiFi.localIP();
    subnet = WiFi.subnetMask();
    broadcast_ip = {0, 255, 255, 255};
    broadcast_ip[0] = ip[0];
  }
  
  #ifdef VERBOSE
    Serial.println(" connected");
    Serial.print("IP: ");
    Serial.println(ip);
    Serial.print("Broadcast IP: ");
    Serial.println(broadcast_ip);
    Serial.print("Subnet: ");
    Serial.println(subnet);
  #endif
}


/* startHotSpot()
 *  This starts our hot spot and webserver.  It doesn't start UDP listner for ArtNet however.
 *  It also resets our device after set timeouts.
 */
void startHotSpot() {
  // Let other functions know we're a hot spot now
  isHotSpot = 1;
  
  // generate the AP ssid & password
  String ssid = nodeName;
  String password = "artNode";
  ssid += '_';
  ssid += String(ESP.getChipId());
  password += String(ESP.getChipId());

  // Turn strings into char array for the functions below
  char ssidChar[ssid.length()];
  ssid.toCharArray(ssidChar, ssid.length());
  char passChar[password.length()];
  password.toCharArray(passChar, password.length());

  // disable wifi and start softAP
  WiFi.disconnect();
  delay(1000);
  WiFi.softAP(ssidChar, passChar);
  
  // Start webServer
  startWebServer();

  // Get MAC Address
  getMac();
  
  ip = WiFi.softAPIP();
  ap_ip = WiFi.softAPIP();
  
  #ifdef VERBOSE
    Serial.print("SSID: ");
    Serial.println(ssidChar);
    Serial.print("Password: ");
    Serial.println(passChar);
  
    Serial.print("AP IP address: ");
    Serial.println(ip);
  
    Serial.print("Waiting for client to connect");
  #endif

  // Set timer
  unsigned long startTime = millis();

  // wait 1 minute for clients to connect
  while((millis() - startTime) < 60000) {
    // Status LEDs
    int val = digitalRead(LED1);
    if (val != HIGH) {
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
    } else {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
    }
    
    // check for any clients connected to the hotspot
    if (wifi_softap_get_station_num() != 0) {
      
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      
      #ifdef VERBOSE
        Serial.println(" done.");
      #endif
      
      // main loop
      while(1) {
        // handle web requests only when in hotSpot mode
        webServer.handleClient();

        // reset timer when a client is connected
        if (wifi_softap_get_station_num() != 0)
          startTime = millis();
        
        // if timer reaches 5 minutes, reset the node
        if ((millis() - startTime) > 300000)
          ESP.restart();

        // yield to core esp functions
        delay(10);
      }
    }
    #ifdef VERBOSE
      Serial.print(".");
    #endif

    // yield to core esp functions
    delay(100);
  }

  // no clients connected within 1 minute - reset device
  #ifdef VERBOSE
    Serial.println(" timeout!");
    Serial.println("Reseting...");
  #endif
  
  ESP.restart();
}


/* getMac()
 *  This gets the MAC address and formats it for later.
 */
void getMac() {
  char MAC_char[30] = "";
  
  WiFi.macAddress(MAC_array);
  
  // Format the MAC address into string
  sprintf(MAC_char, "%02X", MAC_array[0]);
  for (int i = 1; i < 6; ++i)
    sprintf(MAC_char, "%s:%02X", MAC_char, MAC_array[i]);
  MAC_address = String(MAC_char);
}
