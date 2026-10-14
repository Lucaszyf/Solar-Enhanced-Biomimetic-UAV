#include "bsp_servo.h"

Servo myServo_A;  //用于主舵机
Servo myServo_B;  //用于太阳能左舵机
Servo myServo_C;  //用于太阳能右舵机


void servo_init(){
  myServo_A.attach(servo_pin);
  myServo_B.attach(sp_left_spin_pin);
  myServo_C.attach(sp_right_spin_pin);

  // myServo_A.write(100);  //角度加大是往机头方向走  减小是机尾
  setServoAngle(150,0); //垂直角度
  //setServoAngle_sp(105,0);  //设置收缩角度
  myServo_B.write(80);  //右舵机  150-50   180的基础之上对角度做30度的补偿，使得双太阳板旋转角度一致
  myServo_C.write(100);  //右舵机  150-50   180的基础之上对角度做30度的补偿，使得双太阳板旋转角度一致

//逐日模式角度测试，可以在50-150之间，100度可调节  受限于机械结构角度不足以到180度
  // for(int i=50;i<150;i++)setServoAngle_sp(i,50);  //左舵机
  // for(int i=150;i>50;i--)setServoAngle_sp(i,50);  //右舵机
}

// 设置舵机角度并控制转速
void setServoAngle(int angle, int speed) { 
  if(angle<=45){
    angle=45;
  }
  else if(angle>=225){
    angle=225;
  }
  int set_angle = map(angle,0,270,0,180);
  myServo_A.write(set_angle);
  delay(speed);
}

// 设置舵机角度并控制转速
void setServoAngle_sp(int angle, int speed) {
  if(angle<50)angle=50;//界限保护
  else if(angle>150)angle = 150;
  myServo_B.write(angle);  //左舵机  50-150
  myServo_C.write(180-angle);  //右舵机  150-50   180的基础之上对角度做30度的补偿，使得双太阳板旋转角度一致
  delay(speed);
}
void setServoAngle_back(int angle, int speed) {
  if(angle<50)angle=50;//界限保护
  else if(angle>150)angle = 150;
  myServo_B.write(angle);  //左舵机  50-150
  myServo_C.write(180-angle);  //右舵机  150-50   180的基础之上对角度做30度的补偿，使得双太阳板旋转角度一致
  delay(speed);
}