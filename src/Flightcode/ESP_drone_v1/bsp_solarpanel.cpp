#include "bsp_solarpanel.h"
#include "bsp_servo.h"
//太阳能启动
int best_angle; //最佳角度
int current_angle=0; //现在角度
int best_light; //最好光线
int current_light;
int current_light_f,current_light_b;
int default_close_angle=80; //回到机身的角度
int sp_flag;

void sp_close(int sp_speed,int run_time){
  // setServoAngle_back(default_close_angle, 10);
  // analogWrite(sp_pin1, 0);
  // analogWrite(sp_pin2, sp_speed);
  // delay(run_time);//amount of time to close
  // analogWrite(sp_pin1, LOW);
  // analogWrite(sp_pin2, LOW);
  mid_Servo.write(105);
  delay(2000);
  mid_Servo.write(90);
}

void sp_open(int sp_speed,int run_time){
  // setServoAngle_sp(default_close_angle, 10);
  // analogWrite(sp_pin1, sp_speed);
  // analogWrite(sp_pin2, 0);
  // delay(run_time);//amount of time to open
  // analogWrite(sp_pin1, LOW);
  // analogWrite(sp_pin2, LOW);
  mid_Servo.write(75);
  delay(2000);
  mid_Servo.write(90);
}

void sp_angle_cal(){ //追太阳
  //refresh light
  setServoAngle_sp(0, 10);
  while(current_angle<180){
    setServoAngle_sp(current_angle, 10);
    // current_light=analogRead(light_sensor);
    current_angle++;
    if(current_light>=best_light){
      best_light=current_light;
      best_angle=current_angle;
    }
    if(buffer[1]!=2){
      break;
    }
  }
  while(current_angle>0){
    setServoAngle_sp(current_angle, 10);
    // current_light=analogRead(light_sensor);
    current_angle--;
    if(current_light>=best_light){
      best_light=current_light;
      best_angle=current_angle;
    }
    if(buffer[1]!=2){
      break;
    }
  }
}
int sun_state = 0;
int currentAngle = 105; // 初始角度为90度（中间位置）
const int minAngle = 50; // 最小角度
const int maxAngle = 150; // 最大角度

void sun_chark(void)
{
  if(sun_state == 0)
  {
    //sp_open(65,1800);  //太阳能板以50bg速度开
    sun_state = 1;
  }
  current_light_f=analogRead(sp_light_sensor_up);
  current_light_b=analogRead(sp_light_sensor_down) - 1720;
  // 比较两个光敏电阻的值，调整舵机角度
  if (current_light_f - current_light_b> 30 && currentAngle < maxAngle) {
    // 如果上方光照弱，且当前角度小于最大角度，则增加角度
    currentAngle += 2;
  } else if (current_light_b - current_light_f > 30 && currentAngle > minAngle) {
    // 如果下方光照弱，且当前角度大于最小角度，则减少角度
    currentAngle -= 2;
  }
  setServoAngle_sp(currentAngle,50);
  Serial.print("\tcurrent_light_f: ");
  Serial.print(current_light_f);
  Serial.print("\current_light_b: ");
  Serial.println(current_light_b);
  //setServoAngle_sp(currentAngle,50);
}