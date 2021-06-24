#include <LiquidCrystal_I2C.h>
#include "Countimer.h"
Countimer tdown;
Countimer tDown;
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <EEPROM.h>

#define bt_set    6
#define bt_up     5
#define bt_down   9
#define bt_start  10
#define bt_switch  11

#define relay1 12
#define relay2 3

#define buzzer1 4
#define buzzer2 2

const int sensor1 = A0;
const int sensor2 = A1;

int mVperAmp = 185; // use 100 for 20A Module and 66 for 30A Module
double current = 0;
double IRMS = 0; //root 2 is 0.707
double AmpsRMS = 0;

int switch_set = 0;

int time_s1 = 0;
int time_m1= 0;
int time_h1 = 0;

int time_s2 = 0;
int time_m2= 0;
int time_h2 = 0;

int set1 = 0;
int set2 = 0;

int flag_a1=0, flag_a2=0;
int flag_b1=0, flag_b2=0;

bool buzzerState = LOW;

unsigned long previousMillis=0;
unsigned long startTime1 = 0;
unsigned long startTime2 = 0;

int voltage = 220;

void setup() {
Serial.begin (9600);

pinMode(bt_set,   INPUT_PULLUP);
pinMode(bt_up,    INPUT_PULLUP);
pinMode(bt_down,  INPUT_PULLUP);
pinMode(bt_start, INPUT_PULLUP);
pinMode(bt_switch, INPUT_PULLUP);

pinMode(relay1, OUTPUT);
pinMode(relay2, OUTPUT);
pinMode(buzzer1, OUTPUT);
pinMode(buzzer2, OUTPUT);

lcd.begin();
lcd.clear();
lcd.setCursor(0,0);
lcd.print("   Welcome To   ");
lcd.setCursor(0,1);
lcd.print(" Smart Charger ");
tdown.setInterval(print_time1, 999);
tDown.setInterval(print_time2, 999);
eeprom_read1();
eeprom_read2();
delay(1000);
lcd.clear();
}

void print_time1(){
time_s1 = time_s1-1;
if(time_s1<0){time_s1=59; time_m1 = time_m1-1;}
if(time_m1<0){time_m1=59; time_h1 = time_h1-1;}
}

void print_time2(){
time_s2 = time_s2-1;
if(time_s2<0){time_s2=59; time_m2 = time_m2-1;}
if(time_m2<0){time_m2=59; time_h2 = time_h2-1;}
}

void tdownComplete(){Serial.print("ok");}

//tdown.stop(); 

void loop(){
tDown.run();
tdown.run();

if ( digitalRead (bt_switch) == 0) {
  if (flag_a1==0 && flag_a2==0) {
    uint32_t start_time = millis();
    if((millis()-start_time) < 100) {
      flag_a1=1;
      switch_set = switch_set + 1;
//      Serial.println(switch_set);
      if (switch_set >2) {switch_set = 0;}
   } 
  }
} else {
  flag_a1=0;
}

if ( digitalRead (bt_switch) == 0) {
  if (flag_b1==0 && flag_b2==0) {
    uint32_t start_time = millis();
    if((millis()-start_time) < 100) {
      flag_b1=1;
      switch_set = switch_set + 1;
//      Serial.println(switch_set);
      if (switch_set >2) {switch_set = 0;}
    }
  } 
} else {
  flag_b1=0;
}


if(switch_set==1){
  lcd.setCursor(2,0);
  lcd.print(">");
  }
if(switch_set==2){
  lcd.setCursor(2,1);
  lcd.print(">");
  }
  
if(digitalRead (bt_set) == 0){
  if(flag_a1==0 && flag_a2==0 && switch_set == 1){
    uint32_t start_time = millis();
    if((millis()-start_time) < 100) {
    flag_a1=1;
    set1 = set1+1;
//    Serial.println(set1);
    if(set1>3){set1=0;}
    }
}
}else{flag_a1=0;}

if(digitalRead (bt_set) == 0 && switch_set == 2){
  if(flag_b1==0 && flag_b2==0){
    uint32_t start_time = millis();
    if((millis()-start_time) < 100) {
      flag_b1=1;
      set2 = set2+1;
      if(set2>3){set2=0;}
  } 
}
}else{flag_b1=0;}

if(digitalRead (bt_up) == 0 && switch_set == 1){
  uint32_t start_time = millis();
  if((millis()-start_time) < 200) {
    if(set1==0){tdown.start(); flag_a2=1;}
    if(set1==1){time_s1++;}
    if(set1==2){time_m1++;}
    if(set1==3){time_h1++;}
    if(time_s1>59){time_s1=0;}
    if(time_m1>59){time_m1=0;}
    if(time_h1>99){time_h1=0;}
    if(set1>0){eeprom_write1();}
  }
}

if(digitalRead (bt_up) == 0 && switch_set == 2){
  uint32_t start_time = millis();
  if((millis()-start_time) < 200) {
  if(set2==0){tDown.start(); flag_b2=1;}
  if(set2==1){time_s2++;}
  if(set2==2){time_m2++;}
  if(set2==3){time_h2++;}
  if(time_s2>59){time_s2=0;}
  if(time_m2>59){time_m2=0;}
  if(time_h2>99){time_h2=0;}
  if(set2>0){eeprom_write2();}
  }
}

if(digitalRead (bt_down) == 0 && switch_set == 1){
  uint32_t start_time = millis();
  if((millis()-start_time) < 200) {
  if(set1==0){tdown.stop(); flag_a2=0;}
  if(set1==1){time_s1--;}
  if(set1==2){time_m1--;}
  if(set1==3){time_h1--;}
  if(time_s1<0){time_s1=59;}
  if(time_m1<0){time_m1=59;}
  if(time_h1<0){time_h1=99;}
  if(set1>0){eeprom_write1();}
  }
}

if(digitalRead (bt_down) == 0 && switch_set == 2){
  uint32_t start_time = millis();
  if((millis()-start_time) < 200) {
  if(set2==0){tDown.stop(); flag_b2=0;}
  if(set2==1){time_s2--;}
  if(set2==2){time_m2--;}
  if(set2==3){time_h2--;}
  if(time_s2<0){time_s2=59;}
  if(time_m2<0){time_m2=59;}
  if(time_h2<0){time_h2=99;}
  if(set2>0){eeprom_write2();}
  }
}

if(digitalRead (bt_start) == 0 && switch_set == 1){ 
  flag_a2=1; 
  eeprom_read1();
  digitalWrite(relay1, LOW); 
  tdown.restart(); 
  tdown.start();
  startTime1 = millis();
}

if(digitalRead (bt_start) == 0 && switch_set == 2){ 
  flag_b2=1; 
  eeprom_read2();   
  digitalWrite(relay2, LOW); 
  tDown.restart(); 
  tDown.start();
  startTime2 = millis();
}

lcd.setCursor(0,0);
if(set1==0){lcd.print("T1 ");}
lcd.setCursor(6,0);
if(set1==0){lcd.print("  ");}

lcd.setCursor(14,0);
if(set1==1 && switch_set==1){lcd.print("__");}
lcd.setCursor(11,0);
if(set1==2 && switch_set==1){lcd.print("__");}
lcd.setCursor(8,0);
if(set1==3 && switch_set==1){lcd.print("__");}

lcd.setCursor(8,0);
if(time_h1<=9){lcd.print("0");}
lcd.print(time_h1);
lcd.print(":");
if(time_m1<=9){lcd.print("0");}
lcd.print(time_m1);
lcd.print(":");
if(time_s1<=9){lcd.print("0");}
lcd.print(time_s1);
lcd.print("   "); 

lcd.setCursor(0,1);
if(set2==0){lcd.print("T2 ");}
lcd.setCursor(6,1);
if(set2==0){lcd.print("  ");}

lcd.setCursor(14,1);
if(set1==1 && switch_set==2){lcd.print("__");}
lcd.setCursor(11,1);
if(set1==2 && switch_set==2){lcd.print("__");}
lcd.setCursor(8,1);
if(set1==3 && switch_set==2){lcd.print("__");}

lcd.setCursor(8,1);
if(time_h2<=9){lcd.print("0");}
lcd.print(time_h2);
lcd.print(":");
if(time_m2<=9){lcd.print("0");}
lcd.print(time_m2);
lcd.print(":");
if(time_s2<=9){lcd.print("0");}
lcd.print(time_s2);
lcd.print("   "); 

if(time_s1==0 && time_m1==0 && time_h1==0 && flag_a2==1){
  flag_a2=0;
  tdown.stop();
  current = getIPP(sensor1);
  IRMS = (current/2.0) *0.707; //root 2 is 0.707
  AmpsRMS = (IRMS * 1000)/mVperAmp;
  unsigned long endTime1 = millis();
  int duration1 = (endTime1-startTime1)/1000;
  float power = voltage*AmpsRMS*duration1;
  if (power>1000000) {
    power = power/1000000;
    
    lcd.setCursor(3,0);
    lcd.print(power);

    lcd.setCursor(7,0);
    lcd.print("M");
  }
  if (power>1000) {
    power = power/1000;
    
    lcd.setCursor(3,0);
    lcd.print(power);

    lcd.setCursor(7,0);
    lcd.print("K");
  }
  else {
    int power_ = power;
    lcd.setCursor(3,0);
    lcd.print(power_);
  }
  buzzerOn(buzzer1);
}

if(time_s2==0 && time_m2==0 && time_h2==0 && flag_b2==1){
  flag_b2=0;
  tDown.stop(); 
  current = getIPP(sensor2);
  IRMS = (current/2.0) *0.707; //root 2 is 0.707
  AmpsRMS = (IRMS * 1000)/mVperAmp;
  unsigned long endTime2 = millis();
  int duration2 = (endTime2-startTime2)/1000;
  float power = voltage*AmpsRMS*duration2;
  if (power>1000000) {
    power = power/1000000;
    
    lcd.setCursor(3,1);
    lcd.print(power);

    lcd.setCursor(7,1);
    lcd.print("M");
  }
  if (power>1000) {
    power = power/1000;
    
    lcd.setCursor(3,1);
    lcd.print(power);

    lcd.setCursor(7,1);
    lcd.print("K");
  }
  else {
    int power_ = power;
    lcd.setCursor(3,1);
    lcd.print(power_);
  }
  buzzerOn(buzzer2);
}

if(flag_a2==1){digitalWrite(relay1, LOW);}
else{digitalWrite(relay1, HIGH);}

if(flag_b2==1){digitalWrite(relay2, LOW);}
else{digitalWrite(relay2, HIGH);}
//delay(1);
}

void eeprom_write1(){
EEPROM.write(1, time_s1);  
EEPROM.write(2, time_m1);  
EEPROM.write(3, time_h1);  
}

void eeprom_write2(){
EEPROM.write(4, time_s2);  
EEPROM.write(5, time_m2);  
EEPROM.write(6, time_h2);  
}

void eeprom_read1(){
time_s1 =  EEPROM.read(1);
time_m1 =  EEPROM.read(2);
time_h1 =  EEPROM.read(3);
}

void eeprom_read2(){
time_s2 =  EEPROM.read(4);
time_m2 =  EEPROM.read(5);
time_h2 =  EEPROM.read(6);
}

void buzzerOn(int pin){
  unsigned long firstMillis = millis();
    while ((unsigned long)(millis() - firstMillis) <= 500) {
      unsigned long currentMillis = millis(); // grab current time
      // check if "interval" time has passed (1000 milliseconds)
      if ((unsigned long)(currentMillis - previousMillis) >= 100) {
        buzzerState = !buzzerState; // "toggles" the state
        digitalWrite(pin, buzzerState); // sets the LED based on ledState
        // save the "current" time
        previousMillis = millis();
//        Serial.print(previousMillis);
//        Serial.print(" pin:");
//        Serial.println(pin);  
    }
  }
  digitalWrite(pin, LOW);
}

  float getIPP(int numSensor) {
    float result;
    int readValue; //value read from the sensor
    int maxValue = 0; // store max value here
    int minValue = 1024; // store min value here
  
    uint32_t start_time = millis();
    while((millis()-start_time) < 200) //sample for 200ms
    {
      readValue = analogRead(numSensor);
      if (readValue > maxValue) {maxValue = readValue;}
      if (readValue < minValue) {minValue = readValue;}
     }
    result = ((maxValue-minValue)*4.6)/1024.0;
    return result;
}
