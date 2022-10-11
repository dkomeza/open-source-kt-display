#include <OneButton.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#define TFT_CS          15
#define TFT_RST         -1 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC          2
#define TFT_MOSI        13  // Data out
#define TFT_SCLK        27  // Clock out

#define RT0 10000   // Ω
#define B 3950      // K

#define VCC 5
#define R 10000

OneButton buttonUp(0, true);
OneButton buttonDown(1cmd, true);

TFT_eSPI tft = TFT_eSPI(240, 320);

const int BUFFER_SIZE = 12;
const int BUFFER_SIZE_UP = 13;

int batteryLevel = 0;
int previousBatteryLevel = -1;

int power = 0;
int engineTemp = 0; 
int controllerTemp = 0;
int previousEngineTemp = -1; 
int previousControllerTemp = -1; 
int speed = 0;

int currentGear = 0;
int previousGearDisplay = -1;
int previousGearWalk = 0;
int maxGear = 5;
int initialMaxSpeedB2 = 0;
int initialMaxSpeedB4 = 0;

int gearColor = 0;

bool state = false;

byte buf[BUFFER_SIZE_UP];
byte buf_up[BUFFER_SIZE_UP] = {13, 1, 245, 86, 41, 74, 40, 202, 4, 20, 1, 50, 14}; // sample packet generated by the web calculator
// float RT, VR, ln, TX, T0, VRT;
const int samples = 10;

void setup() {
  //Setup serial connection
  // T0 = 25 + 273.15;
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(6);
  initialRender();

  initialMaxSpeedB2 = buf_up[2] & 248;
  initialMaxSpeedB4 = buf_up[4] & 32;

  buttonUp.attachClick(increaseGear);
  buttonDown.attachClick(decreaseGear);
  buttonDown.attachLongPressStart(walkMode);
  buttonDown.attachLongPressStop(stopWalkMode);
  buttonUp.attachLongPressStart(toggleLimit);

  Serial.begin(9600);
  Serial1.begin(9600);
}
 
void loop() {
  long time = millis();
  if (Serial1.available() >= BUFFER_SIZE) { // check if there are enough available bytes to read
    Serial1.readBytes(buf, BUFFER_SIZE); // read bytes to the buf array
  }
  buf_up[1] = currentGear;
  buf_up[5] = calculateCRC(buf_up);
  Serial1.write(buf_up, BUFFER_SIZE_UP);
  buttonUp.tick();
  buttonDown.tick();
  processPacket(buf);
  // getControllerTemperature();
  handleDisplay();
  
  if (millis() - time < 50) {
    delay(50 - (millis() - time));
  }
}

void increaseGear() {
  if (currentGear < maxGear) {
    currentGear++;
  }
  Serial.println("Increase Gear");
} 
void decreaseGear(){       
  if (currentGear > 0) {
    currentGear--;
  }
  Serial.println("Decrease Gear");
}
void walkMode() {
  previousGearWalk = currentGear;
  currentGear = 6;
}
void stopWalkMode() {
  currentGear = previousGearWalk;
}
void toggleLimit() {
  if (state) {
    buf_up[2] = buf_up[2] & 7;
    buf_up[4] = buf_up[4] & 223;
    buf_up[2] = buf_up[2] | initialMaxSpeedB2;
    buf_up[4] = buf_up[4] | initialMaxSpeedB4;
    gearColor = 0;
    updateGear(true, gearColor);
    maxGear = 5;
    state = false;
  }
  else {
    buf_up[2] = buf_up[2] & 7;
    buf_up[4] = buf_up[4] & 223;
    buf_up[2] = buf_up[2] | ((15 & 31) * 8 );
    buf_up[4] = buf_up[4] | (15 & 32);
    gearColor = 1;
    updateGear(true, gearColor);
    if (currentGear > 2) {
      currentGear = 2;
    }
    maxGear = 2;
    state = true;
  }
}

int calculateCRC(byte buf_up[]) {
  int crc = 0;
  for (int i = 0; i < BUFFER_SIZE_UP; i++) {
    if (i != 5) {
      crc ^= buf_up[i];
    }
  }
  crc ^= 3;
  return crc;
}
void processPacket(byte buf[]) {
  // speed = round(60000 / (buf[3] * 256 + buf[4]) * 0.1885 * 0.66 * 10);
  batteryLevel = buf[1];
  power = buf[8] * 13;
  engineTemp = uint8_t(buf[9]) + 15;
}

void handleDisplay() {
  tft.setTextFont(0);
  updateBattery(batteryLevel / 4);
  updateGear(false, gearColor);
  updateEngineTemp();
  // updateControllerTemp();
  updatePower();
  tft.setTextFont(6);
  updateSpeed();
}

void initialRender() {
  // Draw battery
  tft.drawRoundRect(10, 16, 80, 24, 4, TFT_GREEN);
  tft.fillRect(90, 23, 3, 10, TFT_GREEN);
  tft.fillRoundRect(92, 23, 3, 10, 2, TFT_GREEN);

  tft.drawFastHLine(0, 54, 240, TFT_WHITE);
  tft.drawFastHLine(0, 248, 240, TFT_WHITE);
}

void updateBattery(int bars) {
  if (previousBatteryLevel != batteryLevel) {
    for (int i = 0; i < bars; i++) {
      int x = i * 19 + 13;
      tft.fillRoundRect(x, 19, 17, 18, 2, TFT_GREEN);
    }
    previousBatteryLevel = batteryLevel;
  }
}
void updateGear(bool force, int color) {
  if (previousGearDisplay != currentGear || force) {
    tft.setTextSize(8);
    if (color == 1) {
      tft.setTextColor(TFT_GREEN, 0);
    }
    else {
      tft.setTextColor(TFT_YELLOW, 0);
    }
    tft.setCursor(102, 178);
    tft.print(currentGear);
    previousGearDisplay = currentGear;
  }
}
void updateEngineTemp() {
  if (previousEngineTemp != engineTemp) {
    tft.setTextSize(2);
    tft.setCursor(16, 265);
    tft.setTextColor(TFT_WHITE, 0);
    tft.print("MTP:");
    tft.setTextColor(0x007CFF, 0);
    tft.print(engineTemp);
    tft.print("C");
    previousEngineTemp = engineTemp;
  }
}

// void updateControllerTemp() {
//   if (previousControllerTemp >= controllerTemp + 2 || previousControllerTemp <= controllerTemp - 2 ) {
//     tft.setTextSize(2);
//     tft.setCursor(140, 265);
//     tft.setTextColor(TFT_WHITE, 0);
//     tft.print("CTP:");
//     tft.setTextColor(0x007CFF, 0);
//     tft.print(controllerTemp);
//     tft.print("C");
//     previousControllerTemp = controllerTemp;
//   }
// }

void updatePower() {
  tft.setTextSize(2);
  tft.setCursor(16, 290);
  tft.setTextColor(TFT_WHITE, 0);
  tft.print("MOT:");
  tft.setTextColor(0x007CFF, 0);
  tft.print(power);
  tft.println("W");
}
void updateSpeed() {
  tft.setTextColor(TFT_WHITE, 0);
  tft.setCursor(40, 82);
  tft.setTextSize(2);
  int a = 0;
  int b = 0;
  if (speed > 10) {
    a = speed / 10;
    b = speed - (a * 10);
  }
  if (a < 10) {
    tft.print(0);
    tft.print(a);
  }
  else {
    tft.print(a);
  }
  tft.setTextSize(1);
  tft.setCursor(154, 120);
  tft.print(".");
  tft.print(b);
}

// void getControllerTemperature() {
//   float sum = 0;
//   for (int i=0; i < samples; i++) {
//     VRT = analogRead(A7);              //Acquisition analog value of VRT
//     VRT = (5.00 / 1023.00) * VRT;      //Conversion to voltage
//     VR = VCC - VRT;
//     RT = VRT / (VR / R);               //Resistance of RT
  
//     ln = log(RT / RT0);
//     TX = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor
  
//     TX = TX - 273.15;                 //Conversion to Celsius
//     sum += TX;
//   }
//   controllerTemp = sum / samples;
// }






