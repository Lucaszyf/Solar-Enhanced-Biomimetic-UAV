#include "bsp_solarpanel.h"

//太阳能启动
int best_angle; //最佳角度
int current_angle=0; //现在角度
int best_light; //最好光线
int current_light;
int default_close_angle=0; //回到机身的角度
int sp_flag;

void sp_close(){
  setServoAngle_sp(default_close_angle, 10);
  digitalWrite(sp_pin1, HIGH);
  digitalWrite(sp_pin1, LOW);
  delay(0);//amount of time to close
  digitalWrite(sp_pin1, LOW);
  digitalWrite(sp_pin1, LOW);
}

void sp_open(){
  digitalWrite(sp_pin1, LOW);
  digitalWrite(sp_pin1, HIGH);
  delay(0);//amount of time to open
  digitalWrite(sp_pin1, LOW);
  digitalWrite(sp_pin1, LOW);
}

void sp_angle_cal(){ //追太阳
  //refresh light
  setServoAngle_sp(0, 10);
  while(current_angle<180){
    setServoAngle_sp(current_angle, 10);
    current_light=analogRead(light_sensor);
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
    current_light=analogRead(light_sensor);
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