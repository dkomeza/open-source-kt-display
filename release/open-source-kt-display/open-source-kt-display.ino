#include <EEPROM.h>
#include <HardwareSerial.h>
#include <OneButton.h>

#include "src/OTA.h"
#include "src/display/display.h"
#include "src/settings/settings.h"

#define EEPROM_SIZE 512
#define BUFFER_SIZE 12
#define BUFFER_SIZE_UP 13
#define TORQUE_ARRAY_SIZE 40
#define TORQUE_OUTPUT_TABLE_SIZE 5

#define BUTTON_UP 14
#define BUTTON_DOWN 13
#define BUTTON_POWER 12
#define RX_PIN 16
#define TX_PIN 17
#define TORQUE_INPUT_PIN 32
#define TORQUE_OUTPUT_PIN 25
#define BATTERY_INPUT_PIN 33
#define VOLTAGE_REFERENCE_PIN 34

#define TORQUE_OFFSET 1500
#define TORQUE_OUTPUT_MIN 39  // 0.5V (later multiplied by 2 by the opamp to get 1V - min control voltage)

// init buttons
OneButton buttonUp(BUTTON_UP, true);
OneButton buttonDown(BUTTON_DOWN, true);
OneButton buttonPower(BUTTON_POWER, true);

// initialize serial port
HardwareSerial SerialPort(2);

// initialize byte arrays for serial communication
byte buf[BUFFER_SIZE];

// initialize variables
int counter = 0;
int batteryLevel = 0;
int batteryVoltage = 0;
int speed = 0;
int engineTemp = 0;
int controllerTemp = 0;
int power = 0;

// initialize variables for torque sensor
bool enableTorqueSensor = false;
int currentTorque = 0;
int torqueVoltage = 0;
const int torqueOutputTable[TORQUE_OUTPUT_TABLE_SIZE] = {55, 70, 90, 125, 163};

int torqueArray[TORQUE_ARRAY_SIZE];

Settings settings;
Display display;

void setup() {
  // init display
  display.init();
  display.initialRender();

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
  settings.limitState = EEPROM.readBool(20);
  settings.currentGear = EEPROM.read(21);
  settings.enableTorqueSensor = EEPROM.readBool(22);
  if (settings.currentGear > 5) {
    settings.currentGear = 0;
  }

  dacWrite(TORQUE_OUTPUT_PIN, 32);

  // // load settings
  settings.loadSettings();
  settings.calculatePacket();

  // handleLimit();

  // populateTorqueArray();

  // setup serial ports
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17);
  setupOTA("OSKD");

  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(VOLTAGE_REFERENCE_PIN);
  }
  int avg = sum / 10;
  double voltageReference = (double)avg / 4095 * 3.3;
  settings.batteryVoltageOffset = 1.8 - voltageReference;
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

  processPacket();  // process packet from the controller
  getBatteryVoltage();
  if (settings.settingsMenu) {  // render settings menu
    if (millis() - time < 50) {
      delay(50 - (millis() - time));  // delay to make the loop run at a constant rate
    }
  } else {
    // if (validPacket) {       // if the packet is valid render the display, otherwise skip the render
    display.render(batteryLevel, batteryVoltage, speed, engineTemp, controllerTemp, power);
    // }
    if (millis() - time < 50) {
      delay(50 - (millis() - time));  // delay to make the loop run at a constant rate
    }
  }
  SerialPort.write(settings.settings, BUFFER_SIZE_UP);  // send packet to the controller

  buttonUp.tick();
  buttonDown.tick();
  buttonPower.tick();

  ArduinoOTA.handle();
}

/*
 * group of functions for handling with buttons
 */
void handleUpButtonClick() {
  if (settings.settingsMenu) {
    if (settings.cursorPositionCounter > 0 && !settings.selectedOption) {
      settings.cursorPositionCounter--;
      settings.updateCursorPosition();
    } else if (settings.selectedOption) {
      settings.changeSetting(1);
    }
  } else {
    increaseGear();
  }
}
void handleDownButtonClick() {
  if (settings.settingsMenu) {
    if (settings.cursorPositionCounter < MENU_SIZE - 1 && !settings.selectedOption) {
      settings.cursorPositionCounter++;
      settings.updateCursorPosition();
    } else if (settings.selectedOption) {
      settings.changeSetting(-1);
    }
  } else {
    decreaseGear();
  }
}
void handleUpButtonLongPressStart() {
  if (!settings.settingsMenu) {
    // toggleLimit();
  }
}
void handleDownButtonLongPressStart() {
  if (!settings.settingsMenu) {
    startWalkMode();
  }
}
void handleDownButtonLongPressStop() {
  if (!settings.settingsMenu) {
    stopWalkMode();
  }
}
void handlePowerButtonClick() {
  if (settings.settingsMenu) {
    settings.toggleOption();
  } else {
    // toggleTorqueSensor();
  }
}
void handlePowerButtonLongPressStart() {
  if (!settings.settingsMenu) {
    settings.renderSettingsMenu();
    settings.settingsMenu = !settings.settingsMenu;
  } else {
    if (!settings.selectedOption) {
      display.initialRender();
      settings.settingsMenu = !settings.settingsMenu;
      settings.cursorPositionCounter = 0;
      settings.saveSettings();
      settings.calculatePacket();
      display.render(batteryLevel, batteryVoltage, speed, engineTemp, controllerTemp, power);
      display.updateGear(settings.currentGear, settings.gearColor);
    }
  }
}

/*
 * group of functions for dealing with buttons
 */
void increaseGear() {
  if (settings.currentGear < settings.maxGear) {
    settings.currentGear++;
    settings.calculatePacket();
    EEPROM.write(21, settings.currentGear);
    EEPROM.commit();
    display.updateGear(settings.currentGear, settings.gearColor);
  }
}
void decreaseGear() {
  if (settings.currentGear > 0) {
    settings.currentGear--;
    settings.calculatePacket();
    EEPROM.write(21, settings.currentGear);
    EEPROM.commit();
    display.updateGear(settings.currentGear, settings.gearColor);
  }
}
void startWalkMode() {
  settings.walkGear = settings.currentGear;
  settings.currentGear = 6;
  settings.calculatePacket();
  display.updateGear(settings.currentGear, settings.gearColor);
}
void stopWalkMode() {
  settings.currentGear = settings.walkGear;
  settings.calculatePacket();
  display.updateGear(settings.currentGear, settings.gearColor);
}
// void toggleLimit() {
//   limitState = !limitState;
//   EEPROM.writeBool(20, limitState);
//   EEPROM.commit();
//   handleLimit();
// }
// void toggleTorqueSensor() {
//   enableTorqueSensor = !enableTorqueSensor;
//   calculatePacket();
//   saveToLocal();
//   EEPROM.writeBool(22, enableTorqueSensor);
//   EEPROM.commit();
// }

/*
 * group of functions for dealing with the display
 */
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
    settings.gearColor = 2;
  } else {
    settings.gearColor = settings.limitState ? 0 : 1;
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
      int currentCounter = counter + 1;
      shiftArray(currentCounter);
    } else {
      return true;
    }
  }
}
// function to handle "legal mode"
void handleLimit() {
  if (settings.limitState) {
    settings.gearColor = 0;
    if (settings.currentGear > 2) {
      settings.currentGear = 2;
    }
    settings.maxGear = 2;
    settings.speedLimit = 25;
    settings.calculatePacket();
    display.updateGear(settings.currentGear, settings.gearColor);
  } else {
    settings.gearColor = 1;
    settings.maxGear = 5;
    settings.speedLimit = 0;
    settings.calculatePacket();
    display.updateGear(settings.currentGear, settings.gearColor);
  }
}
// function to get battery voltage
void getBatteryVoltage() {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(BATTERY_INPUT_PIN);
  }
  int avg = sum / 10;
  double voltage = map(avg, 0, 4096, 0, 3300);
  // voltage += batteryVoltageOffset;
  double vin = voltage * (1000000000 + 56000000) / 56000000;
  // double vin = voltage / 0.04852521;
  // vin *= 10;
  batteryVoltage = vin / 100;
}

/*
 *  Torque sensor functions
 */
// fill the torque array with 0s
void populateTorqueArray() {
  for (int i = 0; i < TORQUE_ARRAY_SIZE; i++) {
    torqueArray[i] = 0;
  }
}
// function to handle the torque sensor
void handleTorqueSensor() {
  currentTorque = analogRead(TORQUE_INPUT_PIN);
  if (currentTorque > 0) {
    shiftTorqueArray(currentTorque);
  }
  int writeTorque = calculateTorqueOutput(torqueArrayMax());
  if (enableTorqueSensor) {
    dacWrite(TORQUE_OUTPUT_PIN, 32);
  } else {
    dacWrite(TORQUE_OUTPUT_PIN, 32);
  }
}
// add a new value to the torque array and shift the rest of the values
void shiftTorqueArray(int value) {
  for (int i = 0; i < TORQUE_ARRAY_SIZE - 1; i++) {
    torqueArray[i] = torqueArray[i + 1];
  }
  torqueArray[TORQUE_ARRAY_SIZE - 1] = value;
}
// get the maximum value from the last T1/50 values
int torqueArrayMax() {
  int max = 0;
  for (int i = TORQUE_ARRAY_SIZE - (settings.torqueSensorCutOff / 50); i < TORQUE_ARRAY_SIZE; i++) {  // Select the last T1/50 values (T1 - user defined, 50 - 50ms loop time)
    if (torqueArray[i] > max) {
      max = torqueArray[i];
    }
  }
  return max;
}
// map the max torque value to the torque output range
int calculateTorqueOutput(int torque) {
  int writeTorque = map(torque, 0, 4096, 0, 3301);  // map adc readout to voltage (mV)
  writeTorque -= TORQUE_OFFSET;                     // subtract offset
  if (writeTorque < 0 || settings.currentGear == 0) {
    return 0;
  } else if (writeTorque > 1500) {
    writeTorque = 1500;
  }
  return map(writeTorque, 0, 1501, TORQUE_OUTPUT_MIN, torqueOutputTable[settings.currentGear]);
}