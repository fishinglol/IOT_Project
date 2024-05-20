#include<Arduino.h>

/*NOTE 
1. even in repeat trigger mode, PIR "take break" when receiving long signals for 2.1 - 3 second  

 */


//PIR
  /* setting */#define PIR_in_rise 5 
  /* setting */#define PIR_in_fall 17 

  /* public */volatile bool PIR_rise_print_flag = 0;
  /* public */volatile unsigned long PIR_rise_print_flag_time = 0;

  volatile bool PIR_fall_print_flag = 0;

  #define PIR_manage_fLag() if(millis() - PIR_rise_print_flag_time > 3500) if(PIR_rise_print_flag_time) { PIR_rise_print_flag_time = PIR_rise_print_flag = 0;}
  
  void IRAM_ATTR PIR_detect_rise(){
    PIR_rise_print_flag = 1;
    PIR_rise_print_flag_time = 0;   
  }
  void IRAM_ATTR PIR_detect_fall(){
    PIR_rise_print_flag_time = millis();
  }
  void initPIR(){
    pinMode(PIR_in_rise, INPUT_PULLDOWN);
    attachInterrupt(PIR_in_rise, PIR_detect_rise, RISING);//working
    pinMode(PIR_in_fall, INPUT_PULLDOWN);
    attachInterrupt(PIR_in_fall, PIR_detect_fall, FALLING);//working
  }
  
//

 




void setup() {
  Serial.begin(115200);

  initPIR();

}
bool detect = 0;
int start = 0;
void loop() {
  PIR_manage_fLag();


  //test the flag
    if(PIR_rise_print_flag){
      if(!detect){
        detect = 1; 
        start = millis();
      }

      Serial.println(">>>>>>>>>>>>>>>");
      
    } else {
      if(detect){
        Serial.print(millis() - start);
        detect = 0; 
      }
      
      Serial.print(".");
    }
  //

  delay(50);


  
  
}