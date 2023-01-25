#include <EEPROM.h>
#include <HardwareSerial.h>
#include <OneButton.h>

#include "src/OTA.h"
#include "src/display/display.h"
#include "src/settings/settings.h"
#include "src/torqueSensor/torqueSensor.h"

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
TorqueSensor torqueSensor;

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

  // load settings
  settings.loadSettings();
  settings.calculatePacket();

  // handleLimit();

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
    settings.enableTorqueSensor = !settings.enableTorqueSensor
    settings.calculatePacket();
    display.updateTorqueIcon(settigns.enableTorqueSensor);
    EEPROM.writeBool(22, enableTorqueSensor);
    EEPROM.commit();
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