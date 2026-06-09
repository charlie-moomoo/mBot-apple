#include <Arduino.h>
#include "data.h" // Assuming you used the data.h approach

#define SCK_MATRIX A1    
#define DIN_MATRIX A0    

const unsigned int STREAM_SIZE = 29152;

unsigned char drawBuffer[16];
unsigned char drawTemp[16]; 
unsigned long globalByteIndex = 0;

// --- Timing Variables ---
unsigned long lastFrameTime = 0;
const unsigned long FRAME_DELAY_MS = 33; // 1000ms / 30fps = ~33.3ms

// 官方標準 Start
void matrixStart() {
  digitalWrite(SCK_MATRIX, HIGH);
  digitalWrite(DIN_MATRIX, HIGH);
  delayMicroseconds(4);
  digitalWrite(DIN_MATRIX, LOW);
  delayMicroseconds(4);
  digitalWrite(SCK_MATRIX, LOW);
}

// 官方標準 End
void matrixEnd() {
  digitalWrite(SCK_MATRIX, LOW);
  digitalWrite(DIN_MATRIX, LOW);
  delayMicroseconds(4);
  digitalWrite(SCK_MATRIX, HIGH);
  delayMicroseconds(4);
  digitalWrite(DIN_MATRIX, HIGH);
  delayMicroseconds(4);
}

// 官方標準單一位元組發送 (LSB 先發)
void matrixWriteByte(uint8_t data) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(SCK_MATRIX, LOW);
    digitalWrite(DIN_MATRIX, (data & 0x01) ? HIGH : LOW);
    delayMicroseconds(4); 
    digitalWrite(SCK_MATRIX, HIGH);
    delayMicroseconds(4);
    data >>= 1;
  }
  digitalWrite(SCK_MATRIX, LOW); 
}

// 輔助函式：將 8 個位元前後對調
uint8_t reverseBits(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void draw() {
  for (int i = 0; i < 16; i++) {
    drawBuffer[i] = reverseBits(drawTemp[i]);
  }
  
  matrixStart(); matrixWriteByte(0x40); matrixEnd();
  matrixStart(); matrixWriteByte(0xC0); 
  for (int i = 0; i < 16; i++) {
    matrixWriteByte(drawBuffer[i]);
  }
  matrixEnd();
  matrixStart(); matrixWriteByte(0x88); matrixEnd();
}

// Updated: Moves the data stream indexing completely independent of the render clock
void advanceTimeline(unsigned int steps) {
  for (unsigned int i = 0; i < steps; i++) {
    globalByteIndex++;
    
    // When a virtual frame boundary is crossed, wait until the 33ms window opens to draw it
    if (globalByteIndex % 16 == 0) {
      while (millis() - lastFrameTime < FRAME_DELAY_MS) {
        // Active waiting loop handles the precise execution pacing
      }
      draw();
      lastFrameTime = millis(); // Reset clock for the next frame
    }
  }
}

void playCompressedStream() {
  unsigned int streamIndex = 0;
  lastFrameTime = millis(); // Initialize animation clock start

  while (streamIndex < STREAM_SIZE) {
    byte skipCount = pgm_read_byte(&(compressedStream[streamIndex]));
    byte newValue  = pgm_read_byte(&(compressedStream[streamIndex + 1]));
    streamIndex += 2;

    if (skipCount == 255) {
      advanceTimeline((unsigned int)254 * newValue);
      continue;
    }

    advanceTimeline(skipCount);
    drawTemp[globalByteIndex % 16] = newValue;
    advanceTimeline(1); 
  }
}

void setup() {
  pinMode(SCK_MATRIX, OUTPUT);
  pinMode(DIN_MATRIX, OUTPUT);
  
  digitalWrite(SCK_MATRIX, HIGH);
  digitalWrite(DIN_MATRIX, HIGH);
  delay(200);
}

void loop() {
  playCompressedStream();
  
  // Optional: Reset timeline tracker to loop the sequence continuously
  globalByteIndex = 0; 
  delay(1000); // 1-second pause before restarting the loop
}