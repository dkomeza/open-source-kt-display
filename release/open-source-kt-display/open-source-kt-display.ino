#include <EEPROM.h>
#include <HardwareSerial.h>
#include <OneButton.h>
#include <SPI.h>
#include <TFT_eSPI.h>

// defines
#define EEPROM_SIZE 512
#define MENU_SIZE 15
#define BUFFER_SIZE 12
#define BUFFER_SIZE_UP 13

// init buttons
OneButton buttonUp(14, true);
OneButton buttonDown(13, true);
OneButton buttonPower(12, true);

// initialize display
TFT_eSPI tft = TFT_eSPI(240, 320);

// initialize serial port
HardwareSerial SerialPort(2);

// initialize byte arrays for serial communication
byte buf[BUFFER_SIZE];
byte buf_up[BUFFER_SIZE_UP];

// initialize variables
int counter = 0;

// initialize variables
int batteryLevel = 0;
int speed = 0;
int power = 0;
int engineTemp = 0;
int controllerTemp = 0;
int currentGear = 0;

// initialize variables for previous state to prevent unnecessary updates
int previousBatteryLevel = -1;
int previousEngineTemp = -1;
int previousControllerTemp = -1;
int previousGear = -1;
int previousColor = -1;

// initialize variables for "legal mode"
int previousGearWalk = 0;
int maxGear = 5;
int limitState = 0;
int speedLimit = 0;

// gear color - 0: yellow (normal), 1: green ("legal mode"), 2: red (braking)
int gearColor = 0;

bool settingsMenu = false;
bool selectedOption = false;

// initialize variables for settings menu
int cursorPositionCounter = 0;
int cursorPosition[2] = {102, 80};
int previousCursorPosition[2] = {0, 0};

String names[MENU_SIZE] = {"Speed limit", "Wheel size", "P1", "P2", "P3", "P4", "P5", "C1", "C2", "C4", "C5", "C11", "C12", "C13", "C14"};
int values[MENU_SIZE] = {72, 12, 86, 1, 1, 0, 13, 5, 0, 0, 10, 0, 4, 0, 1};
const int minValues[MENU_SIZE] = {10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
const int maxValues[MENU_SIZE] = {72, 14, 255, 6, 1, 1, 30, 7, 1, 4, 10, 3, 7, 5, 3};
const int wheelSizeTable[15][2] = {{50, 22}, {60, 18}, {80, 10}, {100, 14}, {120, 2}, {140, 6}, {160, 0}, {180, 4}, {200, 8}, {230, 12}, {240, 16}, {260, 20}, {275, 24}, {280, 28}, {290, 30}};
byte settings[BUFFER_SIZE_UP];

void setup() {
  // setup display
  tft.init();
  tft.setRotation(6);
  initialRender();

  // initialize eeprom
  EEPROM.begin(EEPROM_SIZE);

  // attach button functions
  buttonUp.attachClick(handleUpButtonClick);
  buttonUp.attachLongPressStart(handleUpButtonLongPressStart);
  buttonDown.attachClick(handleDownButtonClick);
  buttonDown.attachLongPressStart(handleDownButtonLongPressStart);
  buttonDown.attachLongPressStop(handleDownButtonLongPressStop);
  buttonPower.attachClick(handlePowerButtonClick);
  buttonPower.attachLongPressStart(handlePowerButtonLongPressStart);

  // get data from eeprom
  limitState = EEPROM.readBool(20);
  currentGear = EEPROM.read(21);

  // load settings
  getDataFromEEPROM();
  calculatePacket();
  saveToLocal();

  handleLimit();

  // setup serial ports
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17);
}

void loop() {
  long time = millis();

  int SerialAvailableBits = SerialPort.available();
  if (SerialAvailableBits >= BUFFER_SIZE) {  // check if there are enough available bytes to read
    SerialPort.readBytes(buf, BUFFER_SIZE);  // read bytes to the buf array
  } else {
    if (counter > 50) {
      SerialPort.begin(9600, SERIAL_8N1, 16, 17);
      counter = 0;
    }
    counter++;
  }
  bool validPacket = shiftArray(0);

  processPacket();     // process packet from the controller
  if (settingsMenu) {  // render settings menu
    updateCursor(false);
    if (millis() - time < 50) {
      delay(50 - (millis() - time));  // delay to make the loop run at a constant rate
    }
  } else {
    // if (validPacket) {       // if the packet is valid render the display, otherwise skip the render
    handleDisplay(false);  // render normal display without force update
    // }
    if (millis() - time < 50) {
      delay(50 - (millis() - time));  // delay to make the loop run at a constant rate
    }
  }
  updateGear(false, gearColor);

  SerialPort.write(buf_up, BUFFER_SIZE_UP);  // send packet to the controller

  buttonUp.tick();
  buttonDown.tick();
  buttonPower.tick();
}

// group of functions for handling the buttons
void handleUpButtonClick() {
  if (settingsMenu) {
    if (cursorPositionCounter > 0 && !selectedOption) {
      cursorPositionCounter--;
      calculateCursorPosition();
    } else if (selectedOption) {
      handleChange(cursorPositionCounter, "UP");
    }
  } else {
    increaseGear();
  }
}
void handleDownButtonClick() {
  if (settingsMenu) {
    if (cursorPositionCounter < 14 && !selectedOption) {
      cursorPositionCounter++;
      calculateCursorPosition();
    } else if (selectedOption) {
      handleChange(cursorPositionCounter, "DOWN");
    }
  } else {
    decreaseGear();
  }
}
void handleUpButtonLongPressStart() {
  if (!settingsMenu) {
    toggleLimit();
  }
}
void handleDownButtonLongPressStart() {
  if (!settingsMenu) {
    startWalkMode();
  }
}
void handleDownButtonLongPressStop() {
  if (!settingsMenu) {
    stopWalkMode();
  }
}
void handlePowerButtonClick() {
  if (settingsMenu) {
    if (selectedOption) {
      deselectOption(cursorPositionCounter);
    } else {
      selectOption(cursorPositionCounter);
    }
  }
}
void handlePowerButtonLongPressStart() {
  if (!settingsMenu) {
    rendersettingsMenu();
    settingsMenu = !settingsMenu;
  } else {
    if (!selectedOption) {
      initialRender();
      settingsMenu = !settingsMenu;
      cursorPositionCounter = 0;
      saveDataToEEPROM();
      saveToLocal();
      handleDisplay(true);
      updateGear(true, gearColor);
    }
  }
}

// group of functions for dealing with buttons
void increaseGear() {
  if (currentGear < maxGear) {
    currentGear++;
    calculatePacket();
    saveToLocal();
    EEPROM.write(21, currentGear);
    EEPROM.commit();
  }
}
void decreaseGear() {
  if (currentGear > 0) {
    currentGear--;
    calculatePacket();
    saveToLocal();
    EEPROM.write(21, currentGear);
    EEPROM.commit();
  }
}
void startWalkMode() {
  previousGearWalk = currentGear;
  currentGear = 6;
  calculatePacket();
  saveToLocal();
}
void stopWalkMode() {
  currentGear = previousGearWalk;
  calculatePacket();
  saveToLocal();
}
void toggleLimit() {
  limitState = !limitState;
  EEPROM.writeBool(20, limitState);
  EEPROM.commit();
  handleLimit();
}

// group of functions for dealing with the data
// calculating the crc value for the packet to be sent to the controller
int calculateUpCRC(byte packet[]) {
  int crc = 0;
  for (int i = 0; i < BUFFER_SIZE_UP; i++) {
    if (i != 5) {
      crc ^= packet[i];
    }
  }
  crc ^= 3;
  return crc;
}
// calculating the crc value for the packet received from the controller
int calculateDownCRC() {
  int crc = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (i != 6 && i != 0) {
      crc ^= buf[i];
    }
  }
  return crc;
}
// getting readables values from the packet
void processPacket() {
  if (buf[3] + buf[4] <= 0) {
    speed = 0;
  } else {
    speed = round(60000 / (buf[3] * 256 + buf[4]) * 0.1885 * 0.66 * 10);
  }

  if (buf[1] > 16) {
    batteryLevel = 16;
  } else {
    batteryLevel = buf[1];
  }
  power = buf[8] * 13;
  engineTemp = int8_t(buf[9]) + 15;
  if ((buf[7] && 32) == 32) {
    gearColor = 2;
  } else {
    gearColor = limitState ? 0 : 1;
  }
}
// function for shifting the packet in case of a bit loss
bool shiftArray(int counter) {
  int crc = calculateDownCRC();
  if (counter == 5) {
    return false;
  }
  if (buf[0] != 65 || buf[6] != crc) {
    byte newBuf[BUFFER_SIZE];
    int shift = 0;
    for (int i = 1; i < BUFFER_SIZE; i++) {
      if (buf[i] == 65) {
        shift = i;
      }
    }
    shift = BUFFER_SIZE - shift;
    for (int i = 0; i < BUFFER_SIZE; i++) {
      if (i < shift) {
        newBuf[i] = buf[BUFFER_SIZE + i - shift];
      } else {
        newBuf[i] = buf[i - shift];
      }
    }
    memcpy(buf, newBuf, sizeof(newBuf));
    crc = calculateDownCRC();
    if (buf[0] != 65 || buf[6] != crc) {
      int currentCounter = counter += 1;
      shiftArray(currentCounter);
    } else {
      return true;
    }
  }
}
// function to handle "legal mode"
void handleLimit() {
  if (limitState) {
    gearColor = 0;
    updateGear(true, gearColor);
    if (currentGear > 2) {
      currentGear = 2;
    }
    maxGear = 2;
    speedLimit = 25;
    calculatePacket();
    saveToLocal();
  } else {
    gearColor = 1;
    updateGear(true, gearColor);
    maxGear = 5;
    speedLimit = 0;
    calculatePacket();
    saveToLocal();
  }
}

// group of functions for dealing with display
// main function for rendering the display
void handleDisplay(bool force) {
  tft.setTextFont(0);
  updateBattery(batteryLevel / 4, force);
  updateEngineTemp(force);
  updateControllerTemp();
  updatePower();
  updateSpeed();
}
// drawing the empty screen with lines and a battery outline
void initialRender() {
  tft.fillScreen(TFT_BLACK);
  // Draw battery
  tft.drawRoundRect(10, 16, 80, 24, 4, TFT_GREEN);
  tft.fillRect(90, 23, 3, 10, TFT_GREEN);
  tft.fillRoundRect(92, 23, 3, 10, 2, TFT_GREEN);

  tft.drawFastHLine(0, 54, 240, TFT_WHITE);
  tft.drawFastHLine(0, 248, 240, TFT_WHITE);
}
// drawing the battery level bars
void updateBattery(int bars, bool force) {
  if (previousBatteryLevel != batteryLevel || force) {
    tft.fillRect(10, 16, 100, 25, TFT_BLACK);
    tft.drawRoundRect(10, 16, 80, 24, 4, TFT_GREEN);
    tft.fillRect(90, 23, 3, 10, TFT_GREEN);
    tft.fillRoundRect(92, 23, 3, 10, 2, TFT_GREEN);
    for (int i = 0; i < bars; i++) {
      int x = i * 19 + 13;
      tft.fillRoundRect(x, 19, 17, 18, 2, TFT_GREEN);
    }
    previousBatteryLevel = batteryLevel;
  }
}
// drawing the current speed
void updateSpeed() {
  if (speed < 1000) {  // make sure the speed is always 3 digits or less
    tft.setTextColor(TFT_WHITE, 0);
    tft.setCursor(40, 86);
    tft.setTextFont(8);
    tft.setTextSize(1);
    int a = 0;
    int b = 0;
    if (speed > 10) {
      a = speed / 10;
      b = speed - (a * 10);
    }
    if (a < 10) {
      tft.print(0);
      tft.print(a);
    } else {
      tft.print(a);
    }
    tft.setTextFont(6);
    tft.setTextSize(1);
    tft.setCursor(145, 124);
    tft.print(".");
    tft.print(b);
  }
}
// drawing the current gear
void updateGear(bool force, int color) {
  if (previousGear != currentGear || force || previousColor != color) {
    tft.setTextFont(7);
    tft.setTextSize(1);
    if (color == 0) {
      tft.setTextColor(TFT_GREEN, 0);
    } else if (color == 1) {
      tft.setTextColor(TFT_YELLOW, 0);
    } else {
      tft.setTextColor(TFT_RED, 0);
    }
    tft.setCursor(108, 180);
    tft.print(currentGear);
    previousGear = currentGear;
    previousColor = color;
  }
}
// drawing the current engine temperature
void updateEngineTemp(bool force) {
  if (previousEngineTemp != engineTemp || force) {
    tft.setTextSize(2);
    tft.setCursor(16, 265);
    tft.setTextColor(TFT_WHITE, 0);
    tft.print("MTP:");
    tft.setTextColor(TFT_YELLOW, 0);
    if (engineTemp < -10) {
      tft.print(" ");
    } else if (engineTemp < 0) {
      tft.print("  ");
    } else if (engineTemp < 10) {
      tft.print("   ");
    } else if (engineTemp < 100) {
      tft.print("  ");
    } else {
      tft.print(" ");
    }
    tft.print(engineTemp);
    tft.print("C");
    previousEngineTemp = engineTemp;
  }
}
// drawing the current controller temperature
void updateControllerTemp() {
  tft.setTextSize(2);
  tft.setCursor(140, 265);
  tft.setTextColor(TFT_WHITE, 0);
  tft.print("CTP:");
  tft.setTextColor(TFT_YELLOW, 0);
  tft.print(controllerTemp);
  tft.print("C");
  previousControllerTemp = controllerTemp;
}
// drawing the current power draw <to do: make the power more readable (update every 10 times for example)>
void updatePower() {
  tft.setTextSize(2);
  tft.setCursor(16, 290);
  tft.setTextColor(TFT_WHITE, 0);
  tft.print("MP:");
  tft.setTextColor(TFT_YELLOW, 0);
  if (power < 10) {
    tft.print("   ");
  } else if (power < 100) {
    tft.print("  ");
  } else if (power < 1000) {
    tft.print(" ");
  }
  tft.print(power);
  tft.println("W");
}

void rendersettingsMenu() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(4);
  tft.setCursor(16, 16);
  tft.print("settings");
  tft.drawFastHLine(0, 54, 240, TFT_WHITE);
  tft.drawFastHLine(0, 55, 240, TFT_WHITE);
  tft.drawFastHLine(0, 56, 240, TFT_WHITE);
  tft.setTextFont(2);
  tft.drawFastVLine(132, 55, 207, TFT_WHITE);
  for (int i = 0; i < MENU_SIZE; i++) {
    if (i < 9) {
      tft.setCursor(8, 66 + (i * 22));
      tft.drawFastHLine(0, 63 + ((i + 1) * 22), 132, TFT_WHITE);
    } else {
      tft.setCursor(138, 66 + ((i - 9) * 22));
      tft.drawFastHLine(132, 63 + ((i - 8) * 22), 108, TFT_WHITE);
    }
    tft.print(names[i]);
    tft.print(": ");
    if (i == 1) {
      int a = wheelSizeTable[VALUES[i]][0] / 10;
      int b = wheelSizeTable[VALUES[i]][0] - a * 10;

      if (b > 0) {
        tft.print(a);
        tft.print(".");
        tft.print(b);
      } else {
        tft.print(a);
        tft.print("  ");
      }
    } else {
      tft.print(VALUES[i]);
    }
  }
  tft.drawFastHLine(0, 261, 240, TFT_WHITE);
  tft.drawFastHLine(0, 262, 240, TFT_WHITE);
  tft.drawFastHLine(0, 263, 240, TFT_WHITE);
  tft.setCursor(8, 266);
  tft.setTextFont(4);
  tft.print("Packet");
  tft.setTextFont(2);
  calculateCursorPosition();
  calculatePacket();
  drawPacket();
  updateCursor(true);
}

void calculateCursorPosition() {
  if (cursorPositionCounter < 9) {
    cursorPosition[0] = 116;
    cursorPosition[1] = cursorPositionCounter * 22 + 74;
  } else {
    cursorPosition[0] = 202;
    cursorPosition[1] = (cursorPositionCounter - 9) * 22 + 74;
  }
}

void updateCursor(bool force) {
  if (cursorPosition[0] != previousCursorPosition[0] || cursorPosition[1] != previousCursorPosition[1] || force) {
    tft.fillTriangle(previousCursorPosition[0], previousCursorPosition[1], previousCursorPosition[0] + 8, previousCursorPosition[1] + 6, previousCursorPosition[0] + 8, previousCursorPosition[1] - 6, TFT_BLACK);
    tft.fillTriangle(cursorPosition[0], cursorPosition[1], cursorPosition[0] + 8, cursorPosition[1] + 6, cursorPosition[0] + 8, cursorPosition[1] - 6, TFT_WHITE);
    previousCursorPosition[0] = cursorPosition[0];
    previousCursorPosition[1] = cursorPosition[1];
  }
}

void selectOption(int position) {
  selectedOption = true;
  if (position < 9) {
    tft.setCursor(8, 66 + (position * 22));
  } else {
    tft.setCursor(138, 66 + ((position - 9) * 22));
  }
  tft.print(names[position]);
  tft.print(": ");
  tft.setTextColor(TFT_YELLOW, 0);
  if (position == 1) {
    int a = wheelSizeTable[VALUES[position]][0] / 10;
    int b = wheelSizeTable[VALUES[position]][0] - a * 10;
    tft.print(a);
    if (b > 0) {
      tft.print(".");
      tft.print(b);
    } else {
      tft.print("  ");
    }
  } else {
    tft.print(VALUES[position]);
  }
  tft.setTextColor(TFT_WHITE, 0);
}

void deselectOption(int position) {
  selectedOption = false;
  if (position < 9) {
    tft.setCursor(8, 66 + (position * 22));
  } else {
    tft.setCursor(138, 66 + ((position - 9) * 22));
  }
  tft.print(names[position]);
  tft.print(": ");
  if (position == 1) {
    int a = wheelSizeTable[VALUES[position]][0] / 10;
    int b = wheelSizeTable[VALUES[position]][0] - a * 10;
    tft.print(a);
    if (b > 0) {
      tft.print(".");
      tft.print(b);
    } else {
      tft.print("  ");
    }
  } else {
    tft.print(VALUES[position]);
  }
  calculatePacket();
  drawPacket();
}

void handleChange(int position, String direction) {
  if (direction == "UP") {
    if (VALUES[position] < maxValues[position]) {
      values[position]++;
    }
  } else if (direction == "DOWN") {
    if (VALUES[position] > minValues[position]) {
      values[position]--;
    }
  }
  if (position < 9) {
    tft.setCursor(8, 66 + (position * 22));
  } else {
    tft.setCursor(138, 66 + ((position - 9) * 22));
  }
  tft.print(names[position]);
  tft.print(": ");
  tft.setTextColor(TFT_YELLOW, 0);
  if (position == 1) {
    int a = wheelSizeTable[VALUES[position]][0] / 10;
    int b = wheelSizeTable[VALUES[position]][0] - a * 10;
    tft.print(a);
    if (b > 0) {
      tft.print(".");
      tft.print(b);
    } else {
      tft.print("  ");
    }
  } else {
    tft.print(VALUES[position]);
  }
  tft.setTextColor(TFT_WHITE, 0);
}

void calculatePacket() {
  Serial.println("Calculating packet");
  int speed = 0;
  if (speedLimit > 0) {
    speed = speedLimit;
  } else {
    speed = values[0];
  }
  settings[0] = values[6];
  settings[1] = currentGear;
  settings[2] = (((speed - 10) & 31) << 3) | (wheelSizeTable[VALUES[1]][1] >> 2);
  settings[3] = values[2];
  settings[4] = ((wheelSizeTable[VALUES[1]][1] & 3) << 6) | ((speed - 10) & 32) | (VALUES[5] << 4) | values[4] << 3 | values[3];
  settings[5] = 0;
  settings[6] = (VALUES[7] << 3) | values[8];
  settings[7] = (VALUES[14] << 5) | values[10] | 128;
  settings[8] = (VALUES[9] << 5) | values[12];
  settings[9] = 20;
  settings[10] = values[13] << 2 | 1;
  settings[11] = 50;
  settings[12] = 14;
  settings[5] = calculateUpCRC(settings);
}

void getDataFromEEPROM() {
  for (int i = 0; i < 15; i++) {
    values[i] = EEPROM.read(i);
  }
}

void saveDataToEEPROM() {
  for (int i = 0; i < 15; i++) {
    EEPROM.write(i, values[i]);
  }
  EEPROM.commit();
}

void drawPacket() {
  tft.setCursor(8, 280);
  for (int i = 0; i < BUFFER_SIZE_UP; i++) {
    tft.print(settings[i]);
    tft.print(", ");
  }
}

void saveToLocal() {
  for (int i = 0; i < 15; i++) {
    buf_up[i] = settings[i];
  }
}