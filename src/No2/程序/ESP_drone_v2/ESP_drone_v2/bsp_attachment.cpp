// #include "esp32-hal-adc.h"
// #include "esp32-hal-gpio.h"
#include "bsp_attachment.h"

//自动驱动吸盘和爪子
int arm_mode=0;

int last_time;
int current_time;
int count=0;
bool claw_flag=1;
int claw_state=0;

int laser_distance=lasercombined; //laser sensor
int suction_activation_distance; //activate distance
int air_pressure;
int lowest_air_pressure=0;  //最低气压
int target_air_pressure=1; //最高压力

int target_claw_pressure=4095;  //最大爪子压力
int claw_open_button;
int target_claw_time=2000;  //amount of time to close claw

void close_arm(){
    digitalWrite(claw1_pin, HIGH);
    digitalWrite(claw2_pin, LOW);
}

void open_arm(){
    digitalWrite(claw1_pin, LOW);
    digitalWrite(claw2_pin, HIGH);
}


void attach_to_object(){
  if(arm_mode==1){ //吸盘
    //turn on sensors
    air_pressure=analogRead(air_sensor_pin);
    if(air_pressure <= lowest_air_pressure){
      digitalWrite(pump_pin, HIGH);
    }
    else if(air_pressure >= target_air_pressure){
      delay(1000);
      digitalWrite(pump_pin, LOW);
    }
    else ;
  }
  else if(arm_mode==0){ //爪子
    //turn on sensors
    int claw_pressure=analogRead(claw_pressure_pin);
    claw_open_button=digitalRead(claw_open_button_pin);
    if(count==0){
      current_time=millis();
      if(claw_pressure<=target_claw_pressure && claw_flag){  //当实际压力小于目标压力，且不在抓的状态就关爪子
        open_arm();
        claw_flag=0;
        last_time=millis();
        }
      else if (claw_pressure>=target_claw_pressure){  //夹爪压力传感器大于目标设定值，也关闭夹爪电机
        delay(100);
        digitalWrite(claw1_pin, LOW);
        digitalWrite(claw2_pin, LOW);
        count=0;
        claw_flag=1;
        claw_state=1;
        //close sensors
      }
      else if (current_time-last_time>=target_claw_time){  // 超过一定时间关闭夹爪
        delay(100);
        digitalWrite(claw1_pin, LOW);
        digitalWrite(claw2_pin, LOW);
        claw_flag=1;
        count=1;
        claw_state=0;
      }
    }
    else if(count==1){
      digitalWrite(claw1_pin, LOW);  //  开启夹爪
      digitalWrite(claw2_pin, HIGH);
      if(claw_open_button==1){  //中间开关碰到就关闭
        digitalWrite(claw1_pin, LOW);
        digitalWrite(claw2_pin, LOW);
        count=0;
      }
    }
  }
}

void interpret_attach(){
  // arm_mode=digitalRead(arm_mode_sw);

  if(arm_mode==1){  // 吸盘
    laser_distance=lasercombined;
    if(laser_distance>=suction_activation_distance){
      last_time=millis();
      attach_to_object();
    }
    else{
      current_time=millis();
      if(current_time-last_time>=1000){
        digitalWrite(pump_pin, LOW);
        //close sensors
      }
    }
  }
  else if(arm_mode==0){  //爪子
    int claw_sw=digitalRead(claw_sw_pin);  //读取夹爪有没有到最大位置
    if((claw_sw==1 && claw_state!=1)||claw_flag==0){
      attach_to_object();
    }
    if(claw_state==1 && claw_sw==0){
      claw_state=0;
    }
  }
}

void auto_takeoff(){
  delay(3000);
  digitalWrite(e_valve, HIGH); 
  
  delay(1000);
  digitalWrite(e_valve, LOW);
}