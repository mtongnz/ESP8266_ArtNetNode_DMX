bool newScene(char *sceneName) {
  // Open scene list for reading & appending
  File f = SPIFFS.open("/scenes.txt", "a+");
  if (!f)
    return 0;
  
  uint16_t sceneNum = 0;
  
  // Read line by line from the file.  We only need the last number
  while(f.available()) {
    sceneNum = f.readStringUntil(':').toInt();
    f.readStringUntil('\n');
  }

  sceneNum++;

  // Write the scene number and name to our scene list file
  f.print(sceneNum);
  f.print(":");
  f.println(sceneName);
  f.close();

  // Close scene list file
  f.close();

  // Save values.  Return result
  return sceneSave(sceneNum);
}

bool sceneSave(uint16_t sceneNum) {
  // Open scene list for writing - clears file first
  File f = SPIFFS.open("/" + String(sceneNum) + ".save", "w");
  if (!f)
    return 0;

  // Get pointers to our DMX data buffers
  #ifndef VERBOSE
    byte* dmxDataA = dmxA.getChans();
  #else
    byte* dmxDataA = dmxB.getChans();
  #endif
  byte* dmxDataB = dmxB.getChans();

  
  // Get last channel with non-zero value
  uint16_t numChans = dmxA.numChans();
  while (numChans > 0) {
    if (dmxDataA[numChans] != 0)
      break;
    numChans--;
  }
  numChans++;

  // Start file with number of channels in universe A
  f.print(numChans);
  f.print(":");
  
  // Write dmxA channel values
  for (int x = 0; x < numChans; x++)
    f.write(dmxDataA[x]);
    
  yield();
  
  // Get last channel with non-zero value
  numChans = dmxB.numChans();
  while (numChans > 0) {
    if (dmxDataB[numChans] != 0)
      break;
    numChans--;
  }
  numChans++;

  #ifdef VERBOSE
    Serial.print("Store Scene ");
    Serial.println(sceneNum);
    Serial.println("Note: dmxA doesn't work with verbose enabled");
    Serial.print("numChansB: ");
    Serial.println(numChans);
    Serial.print("dmxData: ");
  #endif

  // Print number of channels in universe B
  f.print(":");
  f.print(numChans);
  f.print(":");
  
  // Write dmxB channel values
  for (int x = 0; x < numChans; x++) {
    f.write(dmxDataB[x]);

    #ifdef VERBOSE
      Serial.print(dmxDataB[x]);
      Serial.print(" ");
    #endif
  }
  
  #ifdef VERBOSE
    Serial.println();
  #endif

  // Close file
  f.close();
  
  return 1;
}

bool sceneDelete(uint16_t sceneNum) {
  #ifdef VERBOSE
    Serial.print("Delete Scene ");
    Serial.println(sceneNum);
  #endif

  File a = SPIFFS.open("/scenes.tmp", "w");
  File f = SPIFFS.open("/scenes.txt", "r");
  
  // Copy each line to tmp file - skip our deleted one
  while(f.available()) {
    uint16_t num = f.readStringUntil(':').toInt();
    String tmp = f.readStringUntil('\n');

    if (num != sceneNum) {
      a.print(num);
      a.print(":");
      a.println(tmp);
    }
  }
  
  a.close();
  f.close();

  // Delete save scene and scenes.txt then rename scenes.tmp
  if (SPIFFS.remove("/" + String(sceneNum) + ".save")) {
    if (SPIFFS.remove("/scenes.txt")) {
      if (SPIFFS.rename("/scenes.tmp", "/scenes.txt"))
        return 1;
    }
  }

  // Return 0 if failed
  return 0;
}

bool sceneLoad(uint16_t sceneNum) {
  // Open scene list for reading
  File f = SPIFFS.open("/" + String(sceneNum) + ".save", "r");
  if (!f)
    return 0;

  // get numChans for universe A
  uint16_t numChans = f.readStringUntil(':').toInt();

  byte savedDMX[numChans];

  // Get DMX values from file
  for (int x = 0; x < numChans; x++)
    savedDMX[x] = f.read();

  yield();

  #ifndef VERBOSE
    // Send channel data to DMX output
    dmxA.clearChans();
    yield();
    dmxA.setChans(savedDMX, numChans);
    yield();
  #endif
  
  // Get numChans for universe B
  Serial.println(f.readStringUntil(':'));
  numChans = f.readStringUntil(':').toInt();

  #ifdef VERBOSE
    Serial.print("Load Scene ");
    Serial.println(sceneNum);
    Serial.println("Note: dmxA doesn't work with verbose enabled");
    Serial.print("numChansB: ");
    Serial.println(numChans);
    Serial.print("dmxData: ");
  #endif
  
  byte savedDMXB[numChans];

  // Get DMX values from file
  for (int x = 0; x < numChans; x++) {
    savedDMXB[x] = f.read();
    #ifdef VERBOSE
      Serial.print(savedDMXB[x]);
      Serial.print(" ");
    #endif
  }
  
  #ifdef VERBOSE
    Serial.println();
  #endif

  // Send channel data to DMX output
  yield();
  dmxB.clearChans();
  yield();
  dmxB.setChans(savedDMXB, numChans);

  // Close file
  f.close();

  outputScene = 1;
  outputSceneNum = sceneNum;
  
  return 1;
}

void scenesClear() {
  dmxA.clearChans();
  dmxB.clearChans();
  outputScene = 0;
}

void storeInit() {
  SPIFFS.begin();

  if (!SPIFFS.exists("/scenes.txt") || !SPIFFS.exists("/0.save")) {
    SPIFFS.format();
    
    File f = SPIFFS.open("/scenes.txt", "w");
    f.close();

    f = SPIFFS.open("/0.save", "w");
    f.print("1:");
    f.write(0);
    f.print(":1:");
    f.write(0);
    f.close();
  }
}

