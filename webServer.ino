/*

ESP8266_ArtNetNode_DMX - webServer.ino
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


/////////  Web Page & CSS - stored to flash memory ////////////

const char css[] PROGMEM = "body {text-align:center;background:#333;}\n"
    "table {margin-left:auto;margin-right:auto;max-width:500px;width:100%;border-collapse:collapse;border:none;}\n"
    "th {height:40px;background:#666;color:white;font-weight:bold;border:none;}\n"
    ".mini_head {height:20px;background:#999;}\n"
    "td {padding:6px;border:1px solid #ccc;background:#eee;text-align:left;border:none; position: relative;}\n"
    ".left {width:120px;text-align:right;vertical-align:top;}\n"
    ".centre {text-align: center;}\n"
    "input:not(.button) {float: left;}\n"
    "input:not(.number):not(.radio):not(.button):not(.checkbox) {width: 100%;}\n"
    "#viewWifiPass {width:18px, height:18px; position: absolute; top: 7px; right: 10px; }\n"
    ".number {width:50px;}\n"
    ".button {width:150px;margin:10px;}\n"
    ".smallButton {width:80px;}\n"
    ".static_table {border-collapse:collapse;}\n"
    "p {padding:0px;margin:0px;font-size:14px; clear: both;}\n"
    "a {color:#00A;text-decoration:none;}\n"
    "a:hover {color:#00F;text-decoration:underline;}\n"
    ".bigLink {color: white;}\n"
    ".round-button {display:block; float: left; margin-right: 7px; width:14px; height:14px; font-size:14px; line-height:14px; border-radius: 50%; color:#ffffff; text-align:center; text-decoration:none; background: #5555ff; box-shadow: 0 0 2px #9999ff; font-weight:bold; font-family: 'Comic Sans', serif;}";
    
const char page_head[] PROGMEM = "<html><head><title>ArtNet Node Config</title>\n"
    "<link rel='stylesheet' type='text/css' href='/style.css'>\n"
    "<meta name='viewport' content='width=400'>"
    "</head><body>\n"
    "<table id='wrap'>\n";

const char home_top[] PROGMEM = "<tr><th colspan=5><a href='/' class='bigLink'>WiFi ArtNode Config</a></th></tr>"
    "<form method='POST' action='/save' name='artNodeSettings'>\n";

const char save_top[] PROGMEM = "<tr><th><a href='/' class='bigLink'>WiFi ArtNode Config</a></th></tr><tr><td><center>";

const char save_tail[] PROGMEM = "</center></td></tr></table></body></html>";

const char store_top[] PROGMEM = "<tr><th colspan=5><a href='/' class='bigLink'>WiFi ArtNode Config</a></th></tr>"
    "<form method='POST' action='/store/new.save' name='artNodeStore'>\n";

const char store_tail[] PROGMEM =  "\n<tr><th colspan=5 class='mini_head'>Save a New Scene</th></tr>\n"
    "<tr><td colspan=3><input type=text name='sceneName' value='New Scene'></td>\n"
    "<td colspan=2><input type='submit' value='Save' class='smallButton'></td></tr>\n"
    "</form>\n"
    "</table>\n"
    "</body></html>";
    
const char form_tail[] PROGMEM = "<tr><th colspan=5 class='centre'>\n"
    "<input type='submit' value='Save Changes' class='button'>\n"
    "<input type='submit' name='restart' value='Save & Restart Node' class='button'>\n"
    "</th></tr></form>\n"
    "</table><br />"
    "<table id='wrap'>\n"
    "<tr><th colspan=5><a href='/store' class='bigLink'>Stored Scenes</a></th></tr>\n"
    "</table><br />"
    "<table id='wrap'>\n"
    "<form method='POST' action='/update' enctype='multipart/form-data'>"
    "<tr><th colspan=5>Firmware " FIRMWARE_VERSION "</th></tr>\n"
    "<tr><td colspan=1 class='left'>Update</td>\n"
    "<td colspan=4><input type='file' name='update'></td></tr>"
    "<tr><th colspan=5 class='centre'>\n"
    "<input type='submit' value='Update Firmware' class='button'>"
    "</th></tr></form>"
    "</table></body>\n"
    "<script>\n"
    "var rad = document.artNodeSettings.dhcp;\n"
    "var prev = null;\n"
    "for(var i = 0; i < rad.length; i++) {\n"
    "  rad[i].onclick = function() {\n"
    "    if(this.value != prev)\n"
    "      prev = this.value;\n"
    "      var tab = document.getElementsByClassName('static_table');\n"
    "      if (prev == 'false') {\n"
    "        for(var x = 0; x < tab.length; x++)\n"
    "          tab[x].style.display = 'table-row';\n"
    "      } else {\n"
    "        for(var x = 0; x < tab.length; x++)\n"
    "          tab[x].style.display = 'none';\n"
    "      }\n"
    "      var tab = document.getElementsByClassName('dhcp_table');\n"
    "      if (prev != 'false') {\n"
    "        for(var x = 0; x < tab.length; x++)\n"
    "          tab[x].style.display = 'table-row';\n"
    "      } else {\n"
    "        for(var x = 0; x < tab.length; x++)\n"
    "          tab[x].style.display = 'none';\n"
    "      }\n"
    "   };\n"
    "}\n"
    "</script>\n"
    "</html>";

const char firmware_success[] PROGMEM = "<html><body>"
    "Update successful.  Rebooting...\n"
    "<script>\n"
    "setTimeout(function () {\n"
    "   window.location.href = '/';\n"
    "}, 15000);\n"
    "</script>\n"
    "</body></html>";

const char firmware_fail[] PROGMEM = "<html><body>"
    "Update failed.  Rebooting...\n"
    "<script>\n"
    "setTimeout(function () {\n"
    "   window.location.href = '/';\n"
    "}, 15000);\n"
    "</script>\n"
    "</body></html>";

const char file_fail[] PROGMEM = "<tr><td colspan=5>Failed to read scenes list from file system.</td>";

/* getFlashString()
 *  Get our strings stored in flash memory
 */
String getFlashString(const char *fStr) {
  int len = strlen_P(fStr);
  char buffer[len+1];
  int k;
  
  for (k = 0; k < len; k++)
    buffer[k] =  pgm_read_byte_near(fStr + k);
  
  buffer[k] = 0;
  
  return String(buffer);
}



/* startWebServer()
 *  Very self explanitory - it starts our webserver
 *  Sets the handlers for the various pages we will serve
 */
void startWebServer() {
  webServer.on("/", webHome);
  webServer.on("/save", webSave);
  webServer.on("/style.css", webCSS);
  webServer.on("/update", HTTP_POST, webFirmwareUpdate, webFirmwareUpload);
  webServer.onNotFound(webNotFound);

  //MDNS.begin(wifiSSID);
  webServer.begin();
  //MDNS.addService("http", "tcp", 80);
  
  #ifdef VERBOSE
    Serial.println("HTTP server started");
  #endif
}



/* webHome()
 *  Our main web page.
 */
void webHome() {
  // Stop DMX interupts
  dmxA.pause();
  dmxB.pause();
  
  bool restart = 0;
  
  #ifdef VERBOSE
    Serial.println("HTTP Request Received");
  #endif
  
  // Initialize our page from our flash strings
  String message = getFlashString(page_head);
  message += getFlashString(home_top);

  // Our MAC Address
  message += "<tr><td class='left'>Mac Address</td><td colspan=4>"
        + MAC_address
        + "</td></tr>";

  // ****************** Create our settings form *************
  
  message += "<tr><td class='left'>Node Name</td><td colspan=4>\n"
      "<input type='text' name='nodeName' value='"
      + String(nodeName)
      + "'></td></tr>\n";

  
  // ******* WiFi Settings *********
  
  message += "\n<tr><th colspan=5 class='mini_head'>Wifi</th>\n";

  message += "<tr><td class='left'>SSID</td><td colspan=4>\n"
      "<input type='text' name='wifiSSID' value='"
      + String(wifiSSID)
      + "'></td></tr>\n";

  message += "<tr><td class='left'>Password</td><td colspan=4>\n"
      "<img id='viewWifiPass' src='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABIAAAASCAYAAABWzo5XAAAAAXNSR0IArs4c6QAAAiJJREFUOBHNkzFoU1EUhvNeQl4HQYdECLSgg05aOhTUKWQQCi1iBgkiOrjkJbxmKGpa6JBJkECG2IREcdAhuLiIg1NEsinoIIpEcLLEyUAoVR9J9DuX3HBTIwQnL5x37/nPf/577nn3BgL/w6jVavlKpbIgtVj/WhAiRcuybpD/sdPpnP6rULlcng+HwxcgniDhF6Pt+/6zXC73xRDxB4PBpWw2+/QPISk1GAyWSE4iEjxQ8QDBt8SWwcciwpkQYqfLYPcgHpIgSV2mV9gPsDjzEUwNYndd181p39YLRLYhN7QIeIOjHIO8gl3Ef6S5MsPzyMlrTFUEsEXgtgaZ39DAM5FI5DDH3LBt+xSY9MvHdrHjmB630ul00UIkhchjjco8HA7XM5nMTr1eb+ImBOMowMMkwkdx7wumB3hKjjYuTwcQ/jxan9MY8778HZK+Gphawt+0+/3+VXbrmUECi1RaBJsz8IeyJrZkYFJpD7umelStVuOU/ByCSiTwkwQHX3pyE3tHH15wt6KO47zHj2Ii8h1bow1N9dco+SX4eeybEEYisiwhUKbxLfqV4IK2wLRIF96qiAhRVSQLGZAfMF1XzujDjnskhHDNY74Gv8K1+KS5qiJxRj0RER/SHWY5glQnl1Mf+QPNdqnwrCmiePIRERLkAU5ce3B5ZydJtnhTbc/z2sKfNqxCoRCKxWJPCK7oBziNOBOGWJjdzTszU55J+g2ntvCk6kL2lQAAAABJRU5ErkJggg=='"
      " onClick=\"javascript: var inp = document.getElementById('wifiPass'); var pic = document.getElementById('viewWifiPass');"
      " if (inp.type == 'text') { inp.type = 'password'; pic.src='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABIAAAASCAYAAABWzo5XAAAAAXNSR0IArs4c6QAAAdxJREFUOBHdUjtIQmEYvV5fBUEFOggFLdkQFERkS0O0NEQQDRFBY15RHGqQrMGptgJR1JYgyIbAofYamipoK0inKFBIiB5Qvjvn+l+5We3RD/d+33fO+c7/lKR/Owy/7SwcDndZLJZp8L1CkykWi0d+v//hp55vRtFotNtoNG4ZDIYZNBibmiqoU+VyecXr9d7ruS9G8Xh8HuQOTNooqtVqTwgXzDFGgHcyAf6GsKQoygFrDrkeJAkm6xAmNRPgSWylB+JJfsyJUU8NtbFYbI01h7oimKyC2KhD6v8qm826bDZbO7a5LMtyrVQqbefz+WeHw3EOxZCmxeqCmGhThslck4lUrVZ3Q6FQ2Ww2H8IkiKY15sTIaSaM7KUHtxbQE4K8E9iwjlNzNGqcjpICMpa2CORVjwIbYI24p+G6fFDDhOaFHtoZTQA8xmytQvRYKBT68WYeE4nEODG3232Kt2W3Wq3XKO3EYPCOb8rj8ZyoRgRFQwppB2sIbmHswaGfscYhjwGLAesT/BPyWU7AumHEIhKJOE0m0z4E+rP5IIfRUg/q/xKmC7itjIY13hEBn8+XzuVyLtyMAuGNENFANSFGDqsc1ZtQ92VForERuEK8IyeBSqWS5kQN8s8nn9HVzS14VqbqAAAAAElFTkSuQmCC';"
      " } else { inp.type = 'text'; pic.src='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABIAAAASCAYAAABWzo5XAAAAAXNSR0IArs4c6QAAAiJJREFUOBHNkzFoU1EUhvNeQl4HQYdECLSgg05aOhTUKWQQCi1iBgkiOrjkJbxmKGpa6JBJkECG2IREcdAhuLiIg1NEsinoIIpEcLLEyUAoVR9J9DuX3HBTIwQnL5x37/nPf/577nn3BgL/w6jVavlKpbIgtVj/WhAiRcuybpD/sdPpnP6rULlcng+HwxcgniDhF6Pt+/6zXC73xRDxB4PBpWw2+/QPISk1GAyWSE4iEjxQ8QDBt8SWwcciwpkQYqfLYPcgHpIgSV2mV9gPsDjzEUwNYndd181p39YLRLYhN7QIeIOjHIO8gl3Ef6S5MsPzyMlrTFUEsEXgtgaZ39DAM5FI5DDH3LBt+xSY9MvHdrHjmB630ul00UIkhchjjco8HA7XM5nMTr1eb+ImBOMowMMkwkdx7wumB3hKjjYuTwcQ/jxan9MY8778HZK+Gphawt+0+/3+VXbrmUECi1RaBJsz8IeyJrZkYFJpD7umelStVuOU/ByCSiTwkwQHX3pyE3tHH15wt6KO47zHj2Ii8h1bow1N9dco+SX4eeybEEYisiwhUKbxLfqV4IK2wLRIF96qiAhRVSQLGZAfMF1XzujDjnskhHDNY74Gv8K1+KS5qiJxRj0RER/SHWY5glQnl1Mf+QPNdqnwrCmiePIRERLkAU5ce3B5ZydJtnhTbc/z2sKfNqxCoRCKxWJPCK7oBziNOBOGWJjdzTszU55J+g2ntvCk6kL2lQAAAABJRU5ErkJggg=='; }"
      "\" alt='View/Hide Password' />\n"
      "<input type='password' name='wifiPass' id='wifiPass' value='********'>\n"
      "</td></tr>\n";

  // ******* Hotspot Settings *********
  
  message += "\n<tr><th colspan=5 class='mini_head'>Wifi Hotspot</th>\n";

  message += "<tr><td class='left'>Start Delay</td><td colspan=4>\n"
      "<a href='#' onClick=\"javascript: var help = document.getElementById('helpHotSpotTimeout').style; if (help.display == 'none') {help.display='block';} else { help.display='none';}\" class='round-button'>?</a>\n"
      "<input type='number' name='hotSpotDelay' value='"
      + String(hotSpotDelay)
      + "' min='10' max='65000' class='number'>\n"
      "<p id='helpHotSpotTimeout' style='display:none;'>The hotspot will start after <i>x</i> seconds if it can't connect to the WiFi specified above.  This will only occur on power up and not after a dropped WiFi connection.</p>\n"
      "</td></tr>\n";

  message += "<tr><td class='left'>Stand Alone</td><td colspan = 4>\n"
      "<a href='#' onClick=\"javascript: var help = document.getElementById('helpStandAlone').style; if (help.display == 'none') {help.display='block';} else { help.display='none';}\" class='round-button'>?</a>\n"
      "<input type='checkbox' name='standAlone' value='true' class='checkbox'";
  message += (standAlone == 1) ? " checked>\n" : ">\n";
  message += "<p id='helpStandAlone' style='display:none;'>Disable connecting to WiFi, have hotpot always enabled and enable receiving ArtNet through the hotspot.<br />By default, the device will connect to a WiFi network to receive ArtNet.  HotSpot is only for accessing this settings page.</p>\n"
      "</td></tr>\n";

      
  // ********* ArtNet Settings ********
  
  message += "\n<tr><th colspan=5 class='mini_head'>Artnet</th></tr>\n";

  message += "<tr><td class='left'>Subnet</td><td colspan=4>\n"
      "<input type='number' name='artNetSub' value='"
      + String(artNetSub)
      + "' min=0 max=15 class='number'></td></tr>\n";

  message += "<tr><td class='left'>Universe A</td><td colspan=4>\n"
      "<input type='number' name='artNetUniA' value='"
      + String(artNetUniA)
      + "' min=0 max=15 class='number'></td></tr>\n";

  message += "<tr><td class='left'>Universe B</td><td colspan=4>\n"
      "<input type='number' name='artNetUniB' value='"
      + String(artNetUniB)
      + "' min=0 max=15 class='number'></td></tr>\n";



  // ********** IP Settings ***********

  message += "\n<tr><th colspan=5 class='mini_head'>IP Settings</th></tr>\n";

  message += "<tr><td class='left'></td><td colspan=2>\n"
      "<input type='radio' name='dhcp' value='true'";
  if (dhcp == 1)
    message += " checked";
  message += " class='radio'> &nbsp; "
    "<a href='javascript: document.getElementsByName(\"dhcp\")[0].click();'>DHCP</a>"
    "</td><td colspan=2><input type='radio' name='dhcp' value='false'";
  if (dhcp == 0)
    message += " checked";
  message += " class='radio'> &nbsp; "
    "<a href='javascript: document.getElementsByName(\"dhcp\")[1].click();'>Static</a></td></tr>";

  // IP
  message += "<tr class='static_table'";
  if (dhcp == 1)
    message += " style='display:none;'";
  message += "><td class='left'>IP</td>\n";
  for (int x = 0; x < 4; x++) {
    message += "<td><input type='number' name='ip_";
    message += char(x+48);
    message += "' value='"
        + String(ip[x])
        + "' min=0 max=255 class='number'></td>\n";
  }
  message += "</tr>\n";
  
  message += "<tr class='dhcp_table'";
  if (dhcp == 0)
    message += " style='display:none;'";
  message += "><td class='left'>IP</td>\n";
  message += "<td colspan=4>";
  for (int x = 0; x < 4; x++) {
    if (x > 0)
      message += " . ";
    message += String(ip[x]);
  }
  message += "</td></tr>\n";

  
  
  // Subnet
  message += "<tr class='static_table'";
  if (dhcp == 1)
    message += " style='display:none;'";
  message += "><td class='left'>Subnet</td>\n";
  for (int x = 0; x < 4; x++) {
    message += "<td><input type='number' name='subnet_";
    message += char(x+48);
    message += "' value='"
        + String(subnet[x])
        + "' min=0 max=255 class='number'></td>\n";
  }
  message += "</tr>\n";

  message += "<tr class='dhcp_table'";
  if (dhcp == 0)
    message += " style='display:none;'";
  message += "><td class='left'>Subnet</td>\n";
  message += "<td colspan=4>";
  for (int x = 0; x < 4; x++) {
    if (x > 0)
      message += " . ";
    message += String(subnet[x]);
  }
  message += "</td></tr>\n";
  
  // Broadcast
  message += "<tr class='static_table'";
  if (dhcp == 1)
    message += " style='display:none;'";
  message += "><td class='left'>Broadcast IP</td>\n";
  for (int x = 0; x < 4; x++) {
    message += "<td><input type='number' name='broadcast_ip_";
    message += char(x+48);
    message += "' value='"
        + String(broadcast_ip[x])
        + "' min=0 max=255 class='number'></td>\n";
  }
  message += "</tr>\n";

  message += "<tr class='dhcp_table'";
  if (dhcp == 0)
    message += " style='display:none;'";
  message += "><td class='left'>Broadcast IP</td>\n";
  message += "<td colspan=4>";
  for (int x = 0; x < 4; x++) {
    if (x > 0)
      message += " . ";
    message += String(broadcast_ip[x]);
  }
  message += "</td></tr>\n";

  // Add the end of the form & page
  message += getFlashString(form_tail);

  // Send to the client
  webServer.sendHeader("Connection", "close");
  webServer.send(200, "text/html", message);
  
  // Restart DMX interupts
  dmxA.unPause();
  dmxB.unPause();
}



/* webSave()
 *  Handle the Save buttons being pressed on web page.
 *  Copy data into our global variables.
 *  Verifies data then calls our saveSettings function.
 *  Resets node if the save and reset button clicked
 */
void webSave() {
  // Stop DMX interupts
  dmxA.pause();
  dmxB.pause();
  
  
  #ifdef VERBOSE
    Serial.println("HTTP Save Request includes arguements");
  #endif
  
  char * split;
  char tmpArg[30];
  String message = "";

  // Copy data into our variables
  webServer.arg("nodeName").toCharArray(nodeName, 30);

  // Checkbox for stand alone mode
  if (webServer.hasArg("standAlone"))
    standAlone = 1;
  else
    standAlone = 0;

  // Get numbers
  artNetSub = webServer.arg("artNetSub").toInt();
  artNetUniA = webServer.arg("artNetUniA").toInt();
  artNetUniB = webServer.arg("artNetUniB").toInt();
  hotSpotDelay = webServer.arg("hotSpotDelay").toInt();
  
  // verify that we have valid numbers
  if (artNetSub > 15)
    artNetSub = 15;
  if (artNetUniA > 15)
    artNetUniA = 15;
  if (artNetUniB > 15)
    artNetUniB = 15;
  if (hotSpotDelay < 10)
    hotSpotDelay = 10;
  if (hotSpotDelay > 65000)
    hotSpotDelay = 65000;

  // Copy more data into our variables
  webServer.arg("wifiSSID").toCharArray(wifiSSID, 30);
  if (webServer.arg("wifiPass") != "********")
    webServer.arg("wifiPass").toCharArray(wifiPass, 30);

  // If DHCP - set variables accordingly
  if (webServer.arg("dhcp") == "true") {
    dhcp = 1;

    if (isHotSpot == 1) {
      ip = WiFi.softAPIP();
      ap_ip = WiFi.softAPIP();
    } else {
      ip = WiFi.localIP();
      ap_ip = WiFi.localIP();
    }
    subnet = WiFi.subnetMask();
    broadcast_ip = {0, 255, 255, 255};
    broadcast_ip[0] = ip[0];

  // If Static IP - check IPs first, then set if valid
  } else {
    bool err = 0;
    IPAddress tmp1(0,0,0,0);
    IPAddress tmp2(0,0,0,0);
    IPAddress tmp3(0,0,0,0);
    int x;

    // Check IP
    for (x = 0; x < 4; x++) {
      char id[5] = "ip_";
      id[3] = x+48;
      id[4] = 0;
      webServer.arg(id).toCharArray(tmpArg, 30);
      tmp1[x] = atoi(tmpArg);
      if (tmp1[x] < 0 || tmp1[x] > 255)
        err = 1;
    }
    if (err == 1)
      message += "- Invalid IP Address<br />";
    err = 0;

    // Check broadcast
    for (x = 0; x < 4; x++) {
      char id[15] = "broadcast_ip_";
      id[13] = x+48;
      id[14] = 0;
      webServer.arg(id).toCharArray(tmpArg, 30);
      tmp2[x] = atoi(tmpArg);
      if (tmp2[x] < 0 || tmp2[x] > 255)
        err = 1;
    }
    if (err == 1)
      message += "- Invalid Broadcast Address<br />";
    err = 0;

    // Check subnet
    for (x = 0; x < 4; x++) {
      char id[9] = "subnet_";
      id[7] = x+48;
      id[8] = 0;
      webServer.arg(id).toCharArray(tmpArg, 30);
      tmp3[x] = atoi(tmpArg);
      if (tmp3[x] < 0 || tmp3[x] > 255)
        err = 1;
    }
    if (err == 1)
      message += "- Invalid IP Subnet<br />";
    err = 0;

    // If we dont have error message, set the IP variables
    if (message.length() == 0) {
      // All IPs are valid, store them
      ip = tmp1;
      broadcast_ip = tmp2;
      subnet = tmp3;
      dhcp = 0;
    }
  }

  // Save settings to EEPROM
  if (saveSettings())
    // IP issues from above - the IPs didn't get saved but the rest did
    if (message.length() > 0)
      message = "Some changes saved.  There were the following issues:<br />\n<br />\n" + message;
    else
      message += "Changes Saved.";
      
  // Error saving our settings to EEPROM
  else
    message = "Error saving settings.  Please try again.";

  // If save and reset pressed, notify user that device will reset
  if ( webServer.arg("restart")[0] == 'S')
    message += "<br />\n<br />\nDevice will now restart with it's new settings. Please wait about 30 seconds.\n<br />\n";
  
  message += "<br />\n<a href='/'>Back to settings page</a>";

  //Generate final page using flash strings
  String tmp = getFlashString(page_head);
  tmp += getFlashString(save_top);
  tmp += message;
  tmp += getFlashString(save_tail);

  // Send page
  webServer.sendHeader("Connection", "close");
  webServer.send(200, "text/html", tmp);
  
  // Restart DMX interupts
  dmxA.unPause();
  dmxB.unPause();
  
  // If 'save & reset' was pressed, check for any remaining requests then reset the node
  if ( webServer.arg("restart")[0] == 'S') {
    uint32_t startTime = millis();

    // handle any pending requests
    while (1) {
      webServer.handleClient();
      delay(10);

      // After 5 seconds, exit the while loop
      if ((millis() - startTime) > 5000)
        break;
    }
    
    // reset the node
    #ifdef VERBOSE
      Serial.println("Restarting...");
    #endif
    
    ESP.restart();
  }
}



/* webCSS()
 *  Send our style sheet to the web client
 */
void webCSS() {
  webServer.sendHeader("Connection", "close");
  webServer.send(200, "text/html", getFlashString(css));
}



/* webFirmwareUpdate()
 *  display update status after firmware upload and restart
 */
void webFirmwareUpdate() {
  // Stop DMX so webserver is more responsive
  dmxA.end();
  dmxB.end();
  
  // Generate the webpage from the variables above
  String fail = getFlashString(page_head) + getFlashString(firmware_fail);
  String ok = getFlashString(firmware_success);

  // Send to the client
  webServer.sendHeader("Connection", "close");
  webServer.sendHeader("Access-Control-Allow-Origin", "*");
  webServer.send(200, "text/html", (Update.hasError()) ? fail : ok);

  // Restart device
  delay(200);
  ESP.restart();
}



/* webFirmwareUpload()
 *  handle firmware upload and update
 */
void webFirmwareUpload() {
  // Stop DMX so webserver is more responsive
  dmxA.end();
  dmxB.end();
  
  HTTPUpload& upload = webServer.upload();
  
  if(upload.status == UPLOAD_FILE_START){
    Serial.setDebugOutput(true);
    WiFiUDP::stopAll();
    Serial.printf("Update: %s\n", upload.filename.c_str());
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if(!Update.begin(maxSketchSpace)){//start with max available size
      Update.printError(Serial);
    }
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
      Update.printError(Serial);
    }
  } else if(upload.status == UPLOAD_FILE_END){
    if(Update.end(true)){ //true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      
      // Send success page to the client
      webServer.sendHeader("Connection", "close");
      webServer.sendHeader("Access-Control-Allow-Origin", "*");
      webServer.send(200, "text/html", getFlashString(firmware_success));
    } else {
      Update.printError(Serial);
      
      // Send fail page to the client
      webServer.sendHeader("Connection", "close");
      webServer.sendHeader("Access-Control-Allow-Origin", "*");
      webServer.send(200, "text/html", getFlashString(firmware_fail));
    }
  }
  yield();
}



/* webNotFound()
 *  display a 404 page
 */
void webNotFound() {
  // Check if we're recalling a stored DMX state
  char charBuf[webServer.uri().length() + 1];
  webServer.uri().toCharArray(charBuf, webServer.uri().length() + 1);
  char* tmp = strtok(charBuf, "/.");
  
  if (strcmp(tmp, "store") == 0) {
    webStore();
    return;
  }

  
  #ifdef VERBOSE
    Serial.println("Sending 404");
    Serial.print("URI: ");
    Serial.println(webServer.uri());
    Serial.print("Method: ");
    Serial.println(( webServer.method() == HTTP_GET ) ? "GET" : "POST");
    Serial.print("Arguments: ");
    Serial.println(webServer.args());
  #endif

  // Generate page from flash strings
  String message = getFlashString(page_head);
  message += getFlashString(save_top);
  message += "404: File Not Found\n<br />\n<br />";
  message += "URI: ";
  message += webServer.uri();
  message += "<br />\n<br />\n<a href='/'>Go to settings page</a>";
  message += getFlashString(save_tail);

  // Send page
  webServer.sendHeader("Connection", "close");
  webServer.send(200, "text/html", message);
}



/* webStore()
 *  main page for storing/recalling Artnet data
 */
void webStore() {
  // Stop DMX interupts
  dmxA.pause();
  dmxB.pause();
  
  // Generate page from flash strings
  String message = getFlashString(page_head);
  message += getFlashString(store_top);

  // Get URI Parameteres
  char charBuf[webServer.uri().length() + 1];
  webServer.uri().toCharArray(charBuf, webServer.uri().length() + 1);
  char* tmp = strtok(charBuf, "/.");    // tmp points to store
  tmp = strtok(NULL, "/.");             // tmp points to scene number or new or NULL

  if (tmp != NULL) {
    // Restore Artnet Output
    if (strcmp(tmp, "artnet") == 0) {
      scenesClear();
    
    // Save a new scene
    } else if (strcmp(tmp, "new") == 0) {
      char sceneName[30];
      webServer.arg("sceneName").toCharArray(sceneName, 30);

      bool r = newScene(sceneName);

      if (r == 0) {
        message += "\n<tr><th colspan=5 class='mini_head'>Error</th></tr>\n";
        message += "<tr><td colspan=5><center>Failed to save new scene correctly.</center></td></tr>\n";
      } else {
        message += "\n<tr><th colspan=5 class='mini_head'>Success</th></tr>\n";
        message += "<tr><td colspan=5><center>New scene has been saved.</center></td></tr>\n";
      }
    } else {
      uint8_t sceneNum = atoi(tmp);
      tmp = strtok(NULL, "/.");
      bool r = 0;

      // Call appropriate fuctions
      if (strcmp(tmp, "save") == 0)
        r = sceneSave(sceneNum);
      else if (strcmp(tmp, "delete") == 0)
        r = sceneDelete(sceneNum);
      else if (strcmp(tmp, "load") == 0)
        r = sceneLoad(sceneNum);

      // Handle success and error
      if (r == 0) {
        message += "<tr><td colspan=5><center>ERROR - Scene " + String(sceneNum) + " failed to " + tmp + ".</center></td></tr>\n";
      } else if (strcmp(tmp, "load") != 0) {
        message += "<tr><td colspan=5><center>Success - Scene " + String(sceneNum) + " has been " + tmp + "d.</center></td></tr>\n";
      }
    }
  }

  // Display current output source (Artnet or scene name)
  message += "<tr><td colspan=2>Output Source</td>";
  if (outputScene == 0)
    message += "<td colspan=3>Artnet</td></tr>";
  else {
    if (outputSceneNum == 0)
      message += "<td colspan=3>All Lights Off</td></tr>";
    else {
      File f = SPIFFS.open("/scenes.txt", "r");
      if (f) {
        while(f.available()) {
          uint16_t num = f.readStringUntil(':').toInt();
          String sceneName = f.readStringUntil('\n');
          if (num == outputSceneNum) {
            message += "<td colspan=3>" + sceneName + "</td></tr>\n";
            break;
          }
        }
      }
      f.close();
    }
    message += "<tr><td colspan=2></td><td colspan=3><a href='/store/artnet.restore'>Stop Scene & Resume Artnet</a></td></tr>\n";       
  } 

  // Saved scenes heading and list
  message += "\n<tr><th colspan=5 class='mini_head'>Saved Scenes</th></tr>\n";
  message += "<tr><td colspan=2>All Lights Off</td>\n";
  message += "<td><a href='/store/0.load'>Load</a></td>";
  message += "<td colspan=2><a href='/store/0.save'>Save</a></td></tr>";

  File f = SPIFFS.open("/scenes.txt", "r");
  
  if (!f)
    message += getFlashString(file_fail);
  else {
    // read line by line from the file.  Links for load, save, delete
    while(f.available()) {
      String sceneNum = f.readStringUntil(':');
      String sceneDesc = f.readStringUntil('\n');
      
      message += "<tr><td colspan=2>" + sceneDesc + "</td>";
      message += "<td><a href='/store/" + sceneNum + ".load'>Load</a></td>";
      message += "<td><a href='/store/" + sceneNum + ".save'>Save</a></td>";
      message += "<td><a href='/store/" + sceneNum + ".delete'>Delete</a></td></tr>";
    }
  }
  
  f.close();

  // Display used space
  FSInfo fs_info;
  SPIFFS.info(fs_info);
  message += "<tr><td colspan=5><center><br />";
  message += fs_info.usedBytes;
  message += " of ";
  message += fs_info.totalBytes;
  message += " bytes used.</td></tr>";
  
  message += getFlashString(store_tail);

  // Send page
  webServer.sendHeader("Connection", "close");
  webServer.send(200, "text/html", message);
  
  // Restart DMX interupts
  dmxA.unPause();
  dmxB.unPause();
}
