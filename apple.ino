#include <MeMCore.h>
#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

MeLEDMatrix ledMtx_1(1);
MeRGBLed rgbled_7(7, 2);
unsigned char drawBuffer[16];
unsigned char drawTemp[16];
const int numBytes = 16;
unsigned char data[numBytes];

void draw() {
  memcpy(drawBuffer, drawTemp, 16);
  ledMtx_1.drawBitmap(0, 0, 16, drawBuffer);
}

void setup() {
  ledMtx_1.setColorIndex(1);
  ledMtx_1.setBrightness(1);
  Serial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    rgbled_7.fillPixelsBak(0, 2, 1);
    rgbled_7.setColor(0, 100, 0, 0);
    rgbled_7.show();

    String serialInput = Serial.readStringUntil('\n');
    int startIndex = 0;
    int endIndex;
    int i = 0;

    while ((endIndex = serialInput.indexOf(", ", startIndex)) != -1 && i < numBytes) {
      String valueStr = serialInput.substring(startIndex, endIndex);
      drawTemp[i] = valueStr.toInt();
      i++;
      startIndex = endIndex + 2;
    }

    if (i < numBytes) {
      String lastValueStr = serialInput.substring(startIndex);
      drawTemp[i] = lastValueStr.toInt();
    }

    draw();
  }
  rgbled_7.fillPixelsBak(0, 2, 1);
  rgbled_7.setColor(0, 0, 0, 0);
  rgbled_7.show();
}
