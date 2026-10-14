#include "bsp_usart.h"
unsigned int lasercombined;
const int bufferSize = 64;  // 根据需要调整缓冲区大小
uint8_t buffer[bufferSize];

void usart_rec(void) {
  // 检查是否有可用的数据
  while (Serial2.available()) {
    // 检查帧头
    if (Serial2.read() == 0xAA) {
      // 创建一个数组来存储接收到的数据
      int bytesRead = 0;

      // 读取数据直到帧尾或缓冲区已满
      while (Serial2.available() && bytesRead < bufferSize) {
        uint8_t data = Serial2.read();
        // 将数据存储到缓冲区
        buffer[bytesRead++] = data;
      }
      lasercombined = (buffer[7] << 8) | buffer[6];
    }
  }
}