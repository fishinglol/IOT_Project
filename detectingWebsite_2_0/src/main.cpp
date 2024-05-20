#include<Arduino.h>

// global
  volatile bool from_PIR = 0;
  volatile bool from_HcSr04 = 0; 
  /* output */volatile bool           HcSR04_ready = 0;
  /* output */volatile long           HcSR04_ready_time = 0;
  /* output */volatile bool           PIR_detect = 0;
  /* output */volatile unsigned long  PIR_fall_time = 0;
//


/* working */
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

// HcSR04 /* working */
  /* setting */#define HcSR04_trig 33
  /* setting */#define HcSR04_echo 25
  /* setting */#define HcSR04_buffer_length 5
  /* setting */#define HcSR04_buffer_margin 40
  
  /* public *///volatile bool HcSR04_ready = 0;
  /* public *///volatile long HcSR04_ready_time = 0;
  /* public */volatile unsigned long HcSR04_distance = 0;
  /* public */volatile uint16_t HcSR04_buffer[HcSR04_buffer_length] = {0};
  /* public */void HcSR04_detect() {
     if(PIR_detect){
        from_PIR = 1;
        PIR_detect = 0;
        PIR_fall_time  = 0;
      } 
  }
  
  uint8_t HcSR04_buffer_ptr = 0;
  hw_timer_t *HcSR04_timer;
  volatile bool HcSR04_stable = 1;
  
  #define HcSR04_manage_fLag() if(millis() - HcSR04_ready_time > 500) if(HcSR04_ready_time) { HcSR04_ready_time = HcSR04_ready = 0;}
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

//PIR
  /* setting */#define PIR_in_rise 5 
  /* setting */#define PIR_in_fall 17 

  /* public *///volatile bool PIR_detect = 0;
  /* public *///volatile unsigned long PIR_fall_time = 0;

 

  #define PIR_manage_fLag() if(millis() - PIR_fall_time > 3500) if(PIR_fall_time) { PIR_fall_time = PIR_detect = 0;}
  
  void IRAM_ATTR PIR_detect_rise(){
    if(millis() - PIR_fall_time < 3100) return;
    if(HcSR04_ready){
      from_HcSr04 = 1;
      PIR_fall_time  = PIR_detect = 0;
      HcSR04_ready_time = HcSR04_ready = 0;
      
      
    } else {
      PIR_detect = 1;
      PIR_fall_time = 0;
      
    }
    
  }
  void IRAM_ATTR PIR_detect_fall(){
    //print to console
    if(PIR_detect) PIR_fall_time = millis();
  }
  void initPIR(){
    pinMode(PIR_in_rise, INPUT_PULLDOWN);
    attachInterrupt(PIR_in_rise, PIR_detect_rise, RISING);//working
    pinMode(PIR_in_fall, INPUT_PULLDOWN);
    attachInterrupt(PIR_in_fall, PIR_detect_fall, FALLING);//working
  }
  
//




//web packet
  #include <WiFi.h>       
  #include <WebServer.h>  
  WebServer WEB_server(80);
  /* setting */#define WEB_SSID "Galaxy S9+cd78"
  /* setting */#define WEB_PASSWORD "ivpy2127"
  /* ISR */

  #include"page.h"
  void writePage(){
    /*  */Serial.println("sending web page");
    WEB_server.send(200, "text/html", MAIN_PAGE);// can change 200millisecond to 800
  }
  void sendData() {
    
    String text = "<DATA>";



    if(PIR_detect){
      text += "<PIR_detect>1</PIR_detect>";
    } else {
      text += "<PIR_detect>0</PIR_detect>";
    }

    if(HcSR04_ready){
      text += "<HcSR04_ready>1</HcSR04_ready>";
    } else {
      text += "<HcSR04_ready>0</HcSR04_ready>";
    }
    
    text += "<HcSR04_distance>";
    text += HcSR04_distance ;
    text += "</HcSR04_distance>";

    if(from_HcSr04){
      text += "<direction>Enter</direction>";
      hw508_start(hw508_f_enter);
      from_HcSr04 = 0;
      
    } else if(from_PIR){
      text += "<direction>Exit</direction>";
      hw508_start(hw508_f_exit);
      from_PIR = 0;
      
    } else {
      text += "<direction>0</direction>";
      
    }
    text+= "</DATA>";
    WEB_server.send(200, "text/xml", text);
  }
  /* setting */void WEB_trigger(){
    WEB_server.on("/", writePage);
    WEB_server.on("/REQ_sendData", sendData);
  }

void WEB_init(){
  WiFi.begin(WEB_SSID, WEB_PASSWORD);
  Serial.print("Conneting");
  while(WiFi.status()!= WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nIP address: ");
  Serial.println(WiFi.localIP());

  WEB_trigger();
  WEB_server.begin();
}
//

void setup() {
  Serial.begin(115200);
  Serial.println("\n");

  WEB_init();

  //timer list
  #define timer_hw508 0
  #define timer_HcSR04 1
  //ledc list
  #define ledc_hw508 0

  //init
  hw508_init(timer_hw508, ledc_hw508);
  HcSR04_init(timer_HcSR04);
  initPIR();

}

void loop() {
  WEB_server.handleClient();
  PIR_manage_fLag();
  HcSR04_manage_fLag();
  Serial.println(HcSR04_distance);
  delay(50);

 
}



