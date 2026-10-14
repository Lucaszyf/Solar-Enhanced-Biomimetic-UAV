#include "bsp_SW.h"

int sp_state=0;
int pump_state=1;
int current_servo_angle=135;
int VRB_deviation=0;

void interpret_SW(){
  int SWA=buffer[0];
  int SWB=buffer[1];
  int SWC=buffer[2];
  int SWD=buffer[3];
  int VRA=buffer[4];
  int VRB=buffer[5];

  if(SWB==0&&sp_state!=0){
    sp_close();
    sp_state=0;
  }
  else if(SWB==1&&sp_state==0){
    sp_open();
    sp_state=1;
  }
  else if(SWB==1&&sp_state==2){
    setServoAngle_sp(best_angle, 10);
    sp_state=1;
  }
  else if(SWB==2){
    sp_angle_cal();
    sp_state=2;
  }

  if(SWC==0){
    if(arm_mode==1){
      digitalWrite(pump_pin, HIGH);
      pump_state=0;
    }
    else if(arm_mode==0){
      digitalWrite(claw1_pin, HIGH);
      digitalWrite(claw2_pin, LOW);
    }
  }
  else if(SWC==1){
    if(arm_mode==1){
      if(pump_state!=1){
        digitalWrite(pump_pin, LOW);
        interpret_attach();
        pump_state=1;
      }
      else if(pump_state==1){
        interpret_attach();
      }
    }
    else if(arm_mode==0){
      digitalWrite(claw1_pin, LOW);
      digitalWrite(claw2_pin, LOW);
    }
  }
  else if(SWC==2){
    if(arm_mode==1){
      auto_takeoff();
    }
    else if(arm_mode==0){
      digitalWrite(claw1_pin, LOW);
      digitalWrite(claw2_pin, HIGH);
    }
  }

  if(VRB>150){
    VRB_deviation=200-VRB;
    current_servo_angle--;
    if(current_servo_angle<45){
      current_servo_angle=45;
    }
    setServoAngle(current_servo_angle, VRB_deviation);
  }
  else if(VRB<150){
    if(VRB<100) VRB=100;
    VRB_deviation=VRB-100;
    current_servo_angle++;
    if(current_servo_angle>225){
      current_servo_angle=225;
    }
    setServoAngle(current_servo_angle, VRB_deviation);
  }

  if(VRA>=175){
    //esp screen go right
  }
  else if(VRA<=125){
    //esp screen go left
  }
  //air pressure monitor, and recommmended pressure, and warning when too low pressure
  //auto takeoff notifier
  //claw pressure, and recommended max pressure
  //light/voltage for solar panel and current angle
  //current servo angle

  if(SWA==0){
    digitalWrite(camera_sw, 0);
  }
  else if(SWA==1){
    digitalWrite(camera_sw, 1);
  }
}
