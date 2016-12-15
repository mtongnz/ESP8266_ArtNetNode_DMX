/*

ESP8266_ArtNetNode_DMX - artNet.ino
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



/* checkForNewData()
 *  Checks latest array againt current array for len values.  Returns the index of the last difference.
 */
int checkForNewData(byte *latest, byte *current, int len) {
  #ifdef VERBOSE
    Serial.print("Checking ");
    Serial.print(len + 1);
    Serial.print(" channels for new data... ");
  #endif
  
  // Loop through arrays from the end to find the last difference
  while (--len >= 0) {
    // If a difference is found, return the number of channels to output
    if ( latest[len] != current[len] ) {
      #ifdef VERBOSE
        Serial.print(len);
        Serial.println(" new channels!");
      #endif
      return(len + 1);
    }
  }

  #ifdef VERBOSE
    Serial.println("no new data.");
  #endif
  
  // All data is equal - return 0 channels to output
  return(0);
}



/* artDMXReceived()
 *  Handles ArtNet DMX packets
 */
void artDMXReceived(unsigned char* pbuff) {
  // Don't collect Artnet if we're outputting from stored scenes
  if (outputScene == 1)
    return;

  // Check Subnet
  if ( (pbuff[14] >> 4) != artNetSub )
    return;

  // Which universe is this packet for (Z = neither)
  char uni = ((pbuff[14] & 0xF) == artNetUniA) ? 'A' : (((pbuff[14] & 0xF) == artNetUniB) ? 'B' : 'Z');
  
  // If not correct Artnet Universe
  if ( uni == 'Z' )
    return;
    
  #ifdef VERBOSE
    Serial.print("Artnet Uni ");
    Serial.print(uni);
    Serial.println(" Received");
  #endif
  
  // Number of dimmers hi byte first
  int numberOfDimmers = pbuff[16]*256 + pbuff[17];
  
  // If there's new data, output DMX
  if ( numberOfDimmers > 0 ) {
    if ( uni == 'A')
      dmxA.setChans(&pbuff[ARTNET_ADDRESS_OFFSET], numberOfDimmers);
    
    if ( uni == 'B')
      dmxB.setChans(&pbuff[ARTNET_ADDRESS_OFFSET], numberOfDimmers);
  }
}


/* sendArtNetReply() 
 *  Sends a packet with information such as the Arduino's IP address, univers and subnet settings to other Art-Net
 *  devices listening to the network.
 */
void sendArtNetReply() {
  unsigned char  replyBuffer[ARTNET_REPLY_SIZE];
  int i;

  #ifdef VERBOSE
    Serial.println("Sending Artnet Reply");
  #endif
  
  for ( i=0; i<ARTNET_REPLY_SIZE; i++ ) {
    replyBuffer[i] = 0;
  }
  replyBuffer[0] = 'A';
  replyBuffer[1] = 'r';
  replyBuffer[2] = 't';
  replyBuffer[3] = '-';
  replyBuffer[4] = 'N';
  replyBuffer[5] = 'e';
  replyBuffer[6] = 't';
  replyBuffer[7] = 0;
  replyBuffer[8] = 0;        //op code lo-hi
  replyBuffer[9] = 0x21;      
  replyBuffer[10] = ip[0];      //ip address
  replyBuffer[11] = ip[1];
  replyBuffer[12] = ip[2];
  replyBuffer[13] = ip[3];
  replyBuffer[14] = 0x36;    // port lo first always 0x1936
  replyBuffer[15] = 0x19;
  replyBuffer[16] = 0;      //firmware hi-lo
  replyBuffer[17] = 0;
  replyBuffer[18] = highByte(subnet+1);  //subnet hi-lo
  replyBuffer[19] = lowByte(subnet+1);
  replyBuffer[20] = 0;      //oem hi-lo
  replyBuffer[21] = 0;
  replyBuffer[22] = 0;      // ubea
  replyBuffer[23] = 160;    // status 1
  replyBuffer[24] = 't';    //     ESTA Code (2 bytes)
  replyBuffer[25] = 'm';

                            //short name
  for (int x = 26; x < 43; x++) {
    replyBuffer[x] = nodeName[x-26];
    if (nodeName[x-26] == 0)
      break;
  }
  
                            //long name
  for (int x = 44; x < 70; x++) {
    replyBuffer[x] = nodeName[x-44];
    if (nodeName[x-44] == 0)
      break;
  }
  
  replyBuffer[173] = 2;    //number of ports
  replyBuffer[174] = 128;  //Port 1 can output DMX from network
  replyBuffer[175] = 128;  //Port 2 can output DMX from network
  
  replyBuffer[182] = 128; //Port 1 good output (128 = data being transmitted)
  replyBuffer[183] = 128; //Port 2 good output (128 = data being transmitted)
  
  replyBuffer[190] = artNetUniA + 16*artNetSub;  // Port 1 address
  replyBuffer[191] = artNetUniB + 16*artNetSub;  // Port 2 address

  // MAC Address
  for (int x = 0; x < 6; x++)
    replyBuffer[201 + x] = MAC_array[x];

  replyBuffer[212] = (dhcp) ? 15 : 13;  // status 2

  // Send packet
  eUDP.beginPacket(broadcast_ip, ARTNET_PORT);
  int test = eUDP.write(replyBuffer,ARTNET_REPLY_SIZE);
  eUDP.endPacket();
}


/*  artNetOpCode() 
 *   Checks to see that the packet is actually Art-Net and returns the opcode telling what kind of Art-Net message it is.
 */
int artNetOpCode(unsigned char* pbuff) {
  String test = String((char*)pbuff);
  if ( test.equals("Art-Net") ) {
    if ( pbuff[11] >= 14 ) {            //protocol version [10] hi byte [11] lo byte
      return pbuff[9] *256 + pbuff[8];  //opcode lo byte first
    }
  }
  
  return 0;
}

