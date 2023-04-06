#include <EEPROM.h>
#include <HardwareSerial.h>

#include "src/OTA.h"
#include "src/button/button.h"
#include "src/display/display.h"
#include "src/logic/logic.h"
#include "src/settings/settings.h"
#include "src/torqueSensor/torqueSensor.h"

#define EEPROM_SIZE 512
#define BUFFER_SIZE 12
#define BUFFER_SIZE_UP 13

#define BUTTON_UP 14
#define BUTTON_DOWN 12
#define BUTTON_POWER 27
#define RX_PIN 16
#define TX_PIN 17

#define BATTERY_INPUT_PIN 33
#define VOLTAGE_REFERENCE_PIN 34

int restartCounter = 0;

// init buttons
Button buttonUp = Button(BUTTON_UP);
Button buttonDown = Button(BUTTON_DOWN);
Button buttonPower = Button(BUTTON_POWER);

// initialize serial port
HardwareSerial SerialPort(2);

Display display;
Settings settings;
TorqueSensor torqueSensor;
Logic logic;

void handleUpButtonClick();
void handleDownButtonClick();
void handleUpButtonLongPressStart();
void handleDownButtonLongPressStart();
void handleDownButtonLongPressStop();
void handlePowerButtonClick();
void handlePowerButtonLongPressStart();
void handlePowerButtonDoubleClick();

void increaseGear();
void decreaseGear();
void startWalkMode();
void stopWalkMode();
void toggleLimit();
void toggleTorqueSensor();

int currentState = 0;

void setup() {
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  // init sleep mode
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_POWER, 0);
  pinMode(13, OUTPUT);

  

  // check for the long press on power button on startup
  if (digitalRead(BUTTON_POWER) == LOW) {
    delay(1000);
    if (digitalRead(BUTTON_POWER) == LOW) {
      digitalWrite(5, HIGH);
    } else {
      esp_deep_sleep_start();
    }
  } else {
    esp_deep_sleep_start();
  }

   // init display
  display.init(); 

  // initialize eeprom
  EEPROM.begin(EEPROM_SIZE);

  // attach button functions
  buttonUp.onClick(handleUpButtonClick);
  buttonUp.onLongPressStart(handleUpButtonLongPressStart);
  buttonDown.onClick(handleDownButtonClick);
  buttonDown.onLongPressStart(handleDownButtonLongPressStart);
  buttonDown.onLongPressStop(handleDownButtonLongPressStop);
  buttonPower.onClick(handlePowerButtonClick);
  buttonPower.onLongPressStart(handlePowerButtonLongPressStart);
  buttonPower.onDoubleClick(handlePowerButtonDoubleClick);

  // get data from eeprom
  settings.limitState = EEPROM.readBool(20);
  settings.currentGear = EEPROM.read(21);
  settings.enableTorqueSensor = EEPROM.readBool(22);
  if (settings.currentGear > 5) {
    settings.currentGear = 0;
  }

  // setup serial ports
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17);
  IPAddress IP = setupOTA("OSKD");

  display.renderIP(IP);
  delay(5000);
  display.initialRender();

  // load settings
  settings.loadSettings();
  settings.handleLimit();
  settings.calculatePacket();


  int sum = 0;
  for (int i = 0; i < 100; i++) {
    sum += analogRead(VOLTAGE_REFERENCE_PIN);
  }
  int avg = sum / 100;
  double voltageReference = (double)avg / 4095 * 3.3;
  settings.batteryVoltageOffset = 1.77 - voltageReference;
  display.updateGear(settings.currentGear, settings.gearColor);
}

void loop() {
  long time = millis();

  int SerialAvailableBits = SerialPort.available();
  if (SerialAvailableBits >=
      BUFFER_SIZE) {                               // check if there are enough available bytes to read
    SerialPort.readBytes(logic.buf, BUFFER_SIZE);  // read bytes to the buf array
  } else {
    if (restartCounter > 50) {
      SerialPort.begin(9600, SERIAL_8N1, 16, 17);
      restartCounter = 0;
    }
    restartCounter++;
  }

  bool validPacket = logic.processPacket();  // process the packet
  if (settings.settingsMenu) {               // render settings menu
    if (millis() - time < 50) {
      delay(50 -
            (millis() - time));  // delay to make the loop run at a constant rate
    }
  } else {
    if (validPacket) {  // if the packet is valid render the display,
      // otherwise skip the render
      display.render(logic.batteryLevel, logic.batteryVoltage, logic.speed,
                     logic.engineTemp, 0, logic.power);
    }
    if (millis() - time < 50) {
      delay(50 -
            (millis() - time));  // delay to make the loop run at a constant rate
    }
  }
  SerialPort.write(settings.settings,
                   BUFFER_SIZE_UP);  // send packet to the controller

  torqueSensor.handleTorqueSensor();

  buttonUp.update();
  buttonDown.update();
  buttonPower.update();
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
    if (settings.cursorPositionCounter < MENU_SIZE - 1 &&
        !settings.selectedOption) {
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
    toggleLimit();
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
    settings.enableTorqueSensor = !settings.enableTorqueSensor;
    settings.calculatePacket();
    display.updateTorqueIcon(settings.enableTorqueSensor);
    EEPROM.writeBool(22, settings.enableTorqueSensor);
    EEPROM.commit();
  }
}
void handlePowerButtonLongPressStart() {
  currentState = 1 - currentState;
  digitalWrite(13, currentState);
  if (currentState == 0) {
    esp_deep_sleep_start();
  }
}
void handlePowerButtonDoubleClick() {
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
      display.render(logic.batteryLevel, logic.batteryVoltage, logic.speed,
                     logic.engineTemp, 0, logic.power);
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
void toggleLimit() {
  settings.limitState = !settings.limitState;
  EEPROM.writeBool(20, settings.limitState);
  EEPROM.commit();
  settings.handleLimit();
}
void toggleTorqueSensor() {
  settings.enableTorqueSensor = !settings.enableTorqueSensor;
  settings.calculatePacket();
  EEPROM.writeBool(22, settings.enableTorqueSensor);
  EEPROM.commit();
}