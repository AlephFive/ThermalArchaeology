/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
#include <CapacitiveSensor.h>
// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
// give it a name:


int led = 13;
int capRcv = 2;
int capSend = 11;
int flexPin = A1;

int capPin = A5;

int heatPin1 = 10;
int heatPin2 = 8;

long capValue;
String message;
String valueStr;
//brush
String toolChar = "k";

int flexValue;

int recived;

int numNotHeld = 0;

bool lighton;
bool isHeld;
CapacitiveSensor heldValue = CapacitiveSensor(capSend,capRcv);


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  lighton = true;
  isHeld = false;
  recived = 0;
  flexValue = 0;
  numNotHeld = 0;
  capValue = 0;
  message = toolChar;

  //change this later
  valueStr = "";

  pinMode(led, OUTPUT);
  pinMode(heatPin1, OUTPUT);
  pinMode(heatPin2, OUTPUT);
  Serial.begin(9600);
  unsigned long timeout_millis = 0x1F4;
  heldValue.set_CS_Timeout_Millis(timeout_millis);
  
}

// the loop routine runs over and over again forever:
void loop() {

  //heating element
  digitalWrite(heatPin1, HIGH);
  digitalWrite(heatPin2, LOW);

  
  uint32_t cur_millis = millis();
  static uint32_t sendData_period = 0;
  if (throttle_ms(50, cur_millis, &sendData_period)) {
    
    readCap();
    message += (isHeld)? "1":"0";
    //message += 'c';
    //message += capValue;
    readFlex();
    //message += 'f';
    message += valueStr;

    if(isHeld){
        Serial.print(message);
        //numNotHeld = 0;
        //digitalWrite(led, HIGH);
    }
    else{
      //digitalWrite(led, LOW);
    }
    /*
    else{
      if(numNotHeld <= 5 && numNotHeld > 2){
        Serial.print(message);
      }
      if(numNotHeld < 9999){
        numNotHeld++;
      }
      
    }
    */
    message = toolChar;
    valueStr = "";
  }

  static uint32_t blink_period = 0;
  if (throttle_ms(1000, cur_millis, &blink_period)) {
      //Serial.print("ping");
      
        if(lighton){
          // turn the LED off by making the voltage LOW
          digitalWrite(led, LOW);    
          lighton = false;
        }
        else {
          // turn the LED on
          digitalWrite(led, HIGH);    
          lighton = true;
        }
      
      
    
  }

  if(Serial.available() > 0){
    recived = Serial.read();
    Serial.println(recived, HEX);
    delay(1000);
  }
  
}


bool throttle_ms(uint32_t period_ms, uint32_t cur_time, uint32_t *prev_period) {
  uint32_t cur_period = cur_time / period_ms;
  if (cur_period == *prev_period) {
    return false;  // We're in the same period
  }
  else {
    *prev_period = cur_period;
    return true;
  }
}


void readCap(){
  capValue = analogRead(capPin);
    if(capValue < 500){
      isHeld = false;
    }
    else{
      isHeld = true;
    }
}

void readFlex(){
  flexValue = analogRead(A1);
  valueStr = String(flexValue);

    switch(valueStr.length()){
      case 4:
      
      break;
      case 3:
        valueStr = "0" + valueStr;
      break;
      case 2:
        valueStr = "00" + valueStr;
      break;
      case 1:
        valueStr = "000" + valueStr;
      break;
      default:
        if(valueStr.length()>4){
          valueStr = "high";
        }
        else if(valueStr.length() == 0){
          valueStr = "loww";
        }
        
       
    }
}
