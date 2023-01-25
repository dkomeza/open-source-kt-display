#include "./logic.h"

// calculating the crc value for the packet received from the controller
int Logis::calculateDownCRC() {
  int crc = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (i != 6 && i != 0) {
      crc ^= buf[i];
    }
  }
  return crc;
}
// getting readables values from the packet
void Logis::processPacket() {
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
bool Logis::shiftArray(int counter) {
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