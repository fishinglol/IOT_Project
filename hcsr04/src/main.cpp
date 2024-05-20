
#include<Arduino.h>


// HcSR04 /* working */
  /* setting */#define HcSR04_trig 33
  /* setting */#define HcSR04_echo 25
  /* setting */#define HcSR04_buffer_length 5
  /* setting */#define HcSR04_buffer_margin 10
  
  /* public */volatile bool HcSR04_ready = 0;
  /* public */volatile long HcSR04_ready_time = 0;
  /* public */volatile unsigned long HcSR04_distance = 0;
  /* public */volatile uint16_t HcSR04_buffer[HcSR04_buffer_length] = {0};
  /* public */void HcSR04_detect() {}//insert function to be executed when it detect
  
  uint8_t HcSR04_buffer_ptr = 0;
  hw_timer_t *HcSR04_timer;
  volatile bool HcSR04_stable = 1;
  
  #define HcSR04_manage_fLag() if(millis() - HcSR04_ready_time > 1000) if(HcSR04_ready_time) { HcSR04_ready_time = HcSR04_ready = 0;}
  bool HcSR04_buffer_allsame() {
    for(int i = 1; i<HcSR04_buffer_length ; i++){
      if(HcSR04_buffer[0] - HcSR04_buffer_margin < HcSR04_buffer[i] && HcSR04_buffer[i] < HcSR04_buffer[0] + HcSR04_buffer_margin) ;
      else return 0;
    }
    return 1;
  }
  void HcSR04_scan(){//HcSR04_ready, HcSR04_ready_time, HcSR04_stable, HcSR04_buffer, HcSR04_distance
    digitalWrite(HcSR04_trig, LOW);
    delayMicroseconds(2);
    digitalWrite(HcSR04_trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(HcSR04_trig, LOW);
    HcSR04_distance = pulseIn(HcSR04_echo,HIGH) *17/1000;// 

    //only send HcSR04_ready when going from stable to unstable
    HcSR04_buffer[(HcSR04_buffer_ptr++)%HcSR04_buffer_length] = HcSR04_distance;
    HcSR04_buffer_ptr = (HcSR04_buffer_ptr + 1) % HcSR04_buffer_length;
    if(HcSR04_buffer_allsame()) {
      HcSR04_stable = 1;
    }
    else if(HcSR04_stable){//rising
      HcSR04_stable = 0; 
      HcSR04_detect();
      HcSR04_ready = 1;
      HcSR04_ready_time = millis();
    }
    timerWrite(HcSR04_timer, 9985000);
  }
  void HcSR04_init(uint8_t timer_ID) {
    pinMode(HcSR04_trig, OUTPUT);
    digitalWrite(HcSR04_trig, LOW);
    pinMode(HcSR04_echo, INPUT);
    
    HcSR04_timer = timerBegin(timer_ID, 80, true );
    timerAttachInterrupt(HcSR04_timer, HcSR04_scan, false);
    timerAlarmWrite(HcSR04_timer, 10000000, true); 
    timerAlarmEnable(HcSR04_timer);
    timerWrite(HcSR04_timer, 9990000);
  } 
//


void setup() {
  Serial.begin(115200);

  #define timer_HcSR04 1
  HcSR04_init(timer_HcSR04);

}

bool detect = 0;
uint64_t detect_time = 0;
void loop() {
  
  HcSR04_manage_fLag();

  if(HcSR04_ready){
    if(!detect){
      Serial.println(millis() - detect_time);
      detect_time = millis();
    }
    Serial.println(">>>>>>>>>");
  } else {
    if(!detect){
      Serial.println(millis() - detect_time);
      detect_time = millis();
    }
    Serial.println(".");
  }

 
  
  
  Serial.println(HcSR04_distance);
  delay(50);
  
}
