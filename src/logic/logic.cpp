#include "./logic.h"

int Logic::calculateDownCRC() {
  int crc = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (i != 6 && i != 0) {
      crc ^= buf[i];
    }
  }
  return crc;
}

bool Logic::processPacket() {
  bool validPacket = shiftArray(0);
  batteryVoltage = getBatteryVoltage();
  if (!validPacket) return false;
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
  braking = (buf[7] && 32) == 32;
  return true;
}

bool Logic::shiftArray(int counter) {
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

double Logic::getBatteryVoltage() {
  int sum = 0;
  for (int i = 0; i < 100; i++) {
    sum += analogRead(BATTERY_INPUT_PIN);
  }
  int avg = sum / 100;
  double voltage = map(avg, 0, 4096, 0, 3300);
  voltage += settings.batteryVoltageOffset * 1000;
  double vin = voltage * (1000000000 + 56000000) / 56000000;
  return vin / 100;
}
