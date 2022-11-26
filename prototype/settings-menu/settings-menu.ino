#include <EEPROM.h>
#include <HardwareSerial.h>
#include <OneButton.h>
#include <SPI.h>
#include <TFT_eSPI.h>

// EEPROM defines
#define EEPROM_SIZE 512
#define MENU_SIZE 15

OneButton buttonUp(14, true);
OneButton buttonDown(13, true);
OneButton buttonPower(12, true);

// initialize display
TFT_eSPI tft = TFT_eSPI(240, 320);

// initialize serial port
HardwareSerial SerialPort(2);

int cursorPositionCounter = 0;
int cursorPosition[2] = {102, 80};
int previousCursorPosition[2] = {0, 0};

String NAMES[MENU_SIZE] = {"Speed limit", "Wheel size", "P1", "P2", "P3", "P4", "P5", "C1", "C2", "C4", "C5", "C11", "C12", "C13", "C14"};
int VALUES[MENU_SIZE] = {72, 26, 86, 1, 1, 0, 13, 5, 0, 0, 10, 0, 4, 0, 1};
int MIN_VALUES[MENU_SIZE] = {10, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
int MAX_VALUES[MENU_SIZE] = {72, 29, 255, 6, 1, 1, 30, 7, 1, 4, 10, 3, 7, 5, 3};

bool settingsMenu = false;
bool selectedOption = false;

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

    // setup serial ports
    Serial.begin(9600);
    SerialPort.begin(9600, SERIAL_8N1, 16, 17);
}

void loop() {
    updateCursor();
    // put your main code here, to run repeatedly:
    buttonUp.tick();
    buttonDown.tick();
    buttonPower.tick();
    delay(10);
}

void handleUpButtonClick() {
    if (settingsMenu) {
        if (cursorPositionCounter > 0 && !selectedOption) {
            cursorPositionCounter--;
            calculateCursorPosition();
        } else if (selectedOption) {
            handleChange(cursorPositionCounter, "UP");
        }
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
    }
}

void handleUpButtonLongPressStart() {
    Serial.println("Up button long press start");
}

void handleDownButtonLongPressStart() {
    Serial.println("Down button long press start");
}

void handleDownButtonLongPressStop() {
    Serial.println("Down button long press stop");
}

void handlePowerButtonClick() {
    if (selectedOption) {
        deselectOption(cursorPositionCounter);
    } else {
        selectOption(cursorPositionCounter);
    }
}

void handlePowerButtonLongPressStart() {
    if (!settingsMenu) {
        renderSettingsMenu();
        settingsMenu = !settingsMenu;
    } else {
        if (!selectedOption) {
            initialRender();
            settingsMenu = !settingsMenu;
        }
    }
}

void initialRender() {
    tft.fillScreen(TFT_BLACK);
    // Draw battery
    tft.drawRoundRect(10, 16, 80, 24, 4, TFT_GREEN);
    tft.fillRect(90, 23, 3, 10, TFT_GREEN);
    tft.fillRoundRect(92, 23, 3, 10, 2, TFT_GREEN);

    tft.drawFastHLine(0, 54, 240, TFT_WHITE);
    tft.drawFastHLine(0, 248, 240, TFT_WHITE);
}

void renderSettingsMenu() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextFont(4);
    tft.setCursor(16, 16);
    tft.print("Settings");
    tft.drawFastHLine(0, 54, 240, TFT_WHITE);
    tft.setTextFont(2);
    for (int i = 0; i < MENU_SIZE; i++) {
        if (i < 9) {
            tft.setCursor(8, 72 + (i * 18));
        } else {
            tft.setCursor(128, 72 + ((i - 9) * 18));
        }
        tft.print(NAMES[i]);
        tft.print(": ");
        tft.print(VALUES[i]);
    }
}

void calculateCursorPosition() {
    if (cursorPositionCounter < 9) {
        cursorPosition[0] = 102;
        cursorPosition[1] = cursorPositionCounter * 18 + 80;
    } else {
        cursorPosition[0] = 202;
        cursorPosition[1] = (cursorPositionCounter - 9) * 18 + 80;
    }
}

void updateCursor() {
    if (cursorPosition[0] != previousCursorPosition[0] || cursorPosition[1] != previousCursorPosition[1]) {
        tft.fillTriangle(previousCursorPosition[0], previousCursorPosition[1], previousCursorPosition[0] + 8, previousCursorPosition[1] + 8, previousCursorPosition[0] + 8, previousCursorPosition[1] - 8, TFT_BLACK);
        tft.fillTriangle(cursorPosition[0], cursorPosition[1], cursorPosition[0] + 8, cursorPosition[1] + 8, cursorPosition[0] + 8, cursorPosition[1] - 8, TFT_WHITE);
        previousCursorPosition[0] = cursorPosition[0];
        previousCursorPosition[1] = cursorPosition[1];
    }
}

void selectOption(int position) {
    selectedOption = true;
    if (position < 9) {
        tft.setCursor(8, 72 + (position * 18));
    } else {
        tft.setCursor(128, 72 + ((position - 9) * 18));
    }
    tft.print(NAMES[position]);
    tft.print(": ");
    tft.setTextColor(TFT_YELLOW, 0);
    tft.print(VALUES[position]);
    tft.setTextColor(TFT_WHITE, 0);
}

void deselectOption(int position) {
    selectedOption = false;
    if (position < 9) {
        tft.setCursor(8, 72 + (position * 18));
    } else {
        tft.setCursor(128, 72 + ((position - 9) * 18));
    }
    tft.print(NAMES[position]);
    tft.print(": ");
    tft.print(VALUES[position]);
}

void handleChange(int position, String direction) {
    if (direction == "UP") {
        if (VALUES[position] < MAX_VALUES[position]) {
            VALUES[position]++;
        }
    } else if (direction == "DOWN") {
        if (VALUES[position] > MIN_VALUES[position]) {
            VALUES[position]--;
        }
    }
    if (position < 9) {
        tft.setCursor(8, 72 + (position * 18));
    } else {
        tft.setCursor(128, 72 + ((position - 9) * 18));
    }
    tft.print(NAMES[position]);
    tft.print(": ");
    tft.setTextColor(TFT_YELLOW, 0);
    tft.print(VALUES[position]);
    tft.setTextColor(TFT_WHITE, 0);
}