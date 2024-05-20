
#include<Arduino.h>


//hw508
  /* setting */#define hw508_Vcc 26 
  /* setting */#define hw508_f_enter 698
  /* setting */#define hw508_f_exit 261
  /* setting */#define hw508_res 8
  #define half_duty (1<<(hw508_res/2))
  uint8_t hw508_ledc;
  hw_timer_t *hw508_timer = NULL;
  

  void hw508_start(int frequency){
    ledcSetup(hw508_ledc, frequency, hw508_res);
    ledcWrite(hw508_ledc, half_duty);
    timerWrite(hw508_timer, 0);
    timerStart(hw508_timer);
  }
  void hw508_stop(){
    ledcWrite(hw508_ledc, 0);
    timerStop(hw508_timer);
    timerWrite(hw508_timer, 0);
  }
  void hw508_init(uint8_t _timer_ID, uint8_t _ledc) {
    hw508_ledc = _ledc;
    ledcAttachPin(hw508_Vcc, hw508_ledc);
    
    hw508_timer = timerBegin(_timer_ID, 80, true); 
    timerAttachInterrupt(hw508_timer, hw508_stop, true);
    timerAlarmWrite(hw508_timer, 1000000, true);
    timerAlarmEnable(hw508_timer);
    timerStop(hw508_timer);
    
  }
  

//

void setup() {
  Serial.begin(115200);
  #define timer_hw508 0
  #define ledc_hw508 0

  //init
  hw508_init(timer_hw508, ledc_hw508);
  

}


void loop() {
  hw508_start(hw508_f_enter);
  
  
  delay(3000);
  
}
