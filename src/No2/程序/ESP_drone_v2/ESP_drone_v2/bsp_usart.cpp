#include "bsp_usart.h"
unsigned int lasercombined;
const int bufferSize = 64;  // 根据需要调整缓冲区大小
uint8_t buffer[bufferSize];
uint8_t sun_state_num = 0,sun_state_input = 0,sun_state_output = 0;

void usart_rec1(void)
{
  if (Serial.available() > 0) {
    char receivedData = Serial.read();
    if(receivedData == 'A')
    {
      digitalWrite(4,HIGH);
      digitalWrite(5,LOW);
    }else if(receivedData == 'B')
    {
      digitalWrite(4,LOW);
      digitalWrite(5,HIGH);
    }else if(receivedData == 'C')
    {
      digitalWrite(4,HIGH);
      digitalWrite(5,HIGH);
    }else if(receivedData == 'D')
    {
      digitalWrite(2,LOW);
    }else if(receivedData == 'E')
    {
      digitalWrite(2,HIGH);
    }else if(receivedData == 'O')//自动追阳模式
    {
      sun_state_num = !sun_state_num;
    }else if(receivedData == 'L')//放出太阳能
    {
      sun_state_input = !sun_state_input;
    }else if(receivedData == 'M')//收回太阳能
    {
      sun_state_output = !sun_state_output;
    }
    receivedData = 0;
  }
}

void usart_rec2(void) {
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

