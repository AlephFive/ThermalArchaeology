/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
// give it a name:
int led = 13;
int recived = 0;
bool lighton = true;
// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  
  pinMode(led, OUTPUT);     
  Serial.begin(9600);
  //digitalWrite(led, HIGH);
  
}

// the loop routine runs over and over again forever:
void loop() {
  
  uint32_t cur_millis = millis();
  static uint32_t sendData_period = 0;
  if (throttle_ms(80, cur_millis, &sendData_period)) {
    Serial.print("b11234");
    Serial.print("k05678");
    Serial.print("015p78");
    Serial.print("789");
  }

  static uint32_t blink_period = 0;
  if (throttle_ms(5000, cur_millis, &blink_period)) {
      if(lighton){
        // turn the LED off by making the voltage LOW
        digitalWrite(led, LOW);    
        on = false;
      }
      else{
        // turn the LED on
        digitalWrite(led, HIGH);    
        on = true;
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
