#include <Arduino.h>

#define SCK_MATRIX A1    // 轉接板 S1 -> 接 A1
#define DIN_MATRIX A0    // 轉接板 S2 -> 接 A0

unsigned char drawBuffer[16];
unsigned char drawTemp[16];
const int numBytes = 16;

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

// 輔助函式：將 8 個位元前後對調 (例如 0b00000011 變成 0b11000000)
uint8_t reverseBits(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void draw() {
  // 【上下翻轉修正】
  for (int i = 0; i < 16; i++) {
    drawBuffer[i] = reverseBits(drawTemp[i]);
  }
  
  matrixStart(); matrixWriteByte(0x40); matrixEnd();
  matrixStart(); matrixWriteByte(0xC0); 
  for (int i = 0; i < 16; i++) {
    matrixWriteByte(drawBuffer[i]);
  }
  matrixEnd();
  matrixStart(); matrixWriteByte(0x89); matrixEnd();
}


void setup() {
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SCK_MATRIX, OUTPUT);
  pinMode(DIN_MATRIX, OUTPUT);
  
  // 讓硬體回到安全初始高電位
  digitalWrite(SCK_MATRIX, HIGH);
  digitalWrite(DIN_MATRIX, HIGH);
  delay(200);

  // ==========================================
  // ✨ 【新增】開機檢測效果：全亮 ➔ 等待 1 秒 ➔ 清空
  // ==========================================
  
  // 步驟 1：將快取全部填滿 255 (全亮) 並畫上去
  for(int i = 0; i < 16; i++) {
    drawTemp[i] = 255;
  }
  draw();
  
  // 步驟 2：維持全亮狀態 1 秒鐘 (1000 毫秒)
  delay(1000);
  
  // 步驟 3：將快取全部清空為 0 (全暗) 並畫上去
  for(int i = 0; i < 16; i++) {
    drawTemp[i] = 0;
  }
  draw();
}

void loop() {
  if (Serial.available()) {
    digitalWrite(LED_BUILTIN, HIGH);

    String serialInput = Serial.readStringUntil('\n');
    serialInput.trim(); 
    
    int byteCount = 0;
    int fromIndex = 0;
    
    while (byteCount < numBytes) {
      int commaIndex = serialInput.indexOf(',', fromIndex);
      
      if (commaIndex != -1) {
        String subStr = serialInput.substring(fromIndex, commaIndex);
        subStr.trim();
        drawTemp[byteCount] = subStr.toInt();
        fromIndex = commaIndex + 1;
        byteCount++;
      } else {
        String subStr = serialInput.substring(fromIndex);
        subStr.trim();
        if (subStr.length() > 0) {
          drawTemp[byteCount] = subStr.toInt();
          byteCount++;
        }
        break;
      }
    }

    if (byteCount == numBytes) {
      draw();
      Serial.println("OK: Matrix Updated.");
    } else {
      Serial.print("Error: Received ");
      Serial.print(byteCount);
      Serial.println(" bytes.");
    }
    
    digitalWrite(LED_BUILTIN, LOW);
  }
}
