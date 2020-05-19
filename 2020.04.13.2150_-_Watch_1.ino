//2020.04.13.2150 - INDS4007 - Watch 1
//A part of the Corporeal Instrumentality Project
//Uses the L293D quad half-bridge motor driver
//Uses a pulse sensor.
//Precedents: 2020.01.21.1713 Pulse 12, 2020.03.13.2526 Stepper 1

/* This code tracks the wearer's pulse and causes the Lavet-type
 *  stepper motor in a quartz wristwatch to advance by one second
 *  every time a heartbeat is detected.
 */

const int motorPinA = 7;
const int motorPinB = 8;
const int pulsePin = A0;

//variables for the pulse reading
int reading; //stores analogue reading from the pulsePin
int threshold = 550; //pressure required for pulse; 0 < threshold < 1024

//variables for the pulse's edge detection
bool beating = false; //is "true" if reading > threshold
bool lastBeat = false; //previous beat

//variables for the pulse interval calculation
bool firstBeatDetected = false;
unsigned long firstBeatTime = 0;
unsigned long secondBeatTime = 0;
unsigned long beatInterval = 0;
const int intervalMin = 400; //erroneously low pulse interval is discarded
  //400 ms == 150 bpm, which is pretty fast already
//const int intervalMax = 1100; //erroneously high pulse interval is discarded


//variables for the motor
const int onTime = 20; //milliseconds per "on" cycle
int motorState = 0; //indicates whether pinA/B was last-used
/*The Lavet-type stepper motor must be powered with current
 * flowing in alternating directions. Every time the current
 * changes direction, the motor will advance one step, or 1/60
 * of a revolution: a "second." Thus in order to keep the motor
 * going, we must remember the previous direction of current.
 */


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(motorPinA, OUTPUT);
  pinMode(motorPinB, OUTPUT);
  pinMode(pulsePin, INPUT);
}

void loop() {
  //read pressure from pulse sensor
  reading = analogRead(pulsePin);
  Serial.println(reading);

  //edge detection: detect if it's the start of a beat
  if(reading > threshold && lastBeat == false){
    beating = true;
    
      //calculate time interval between two most recent beats
      //this occurs only once, before an interval can be measured
      if(firstBeatDetected == false){
        firstBeatTime = millis();
        firstBeatDetected = true;
      }
      //this occurs on all subsequent cycles
      else{
        secondBeatTime = millis();
        beatInterval = secondBeatTime - firstBeatTime;
        
        //actuate motor only if enough time has passed since last beat
          if(intervalMin < beatInterval){
            //Serial.println("beat!");
            //alternate the direction of current on every beat
            if(motorState == 0){
              digitalWrite(motorPinA, HIGH);
              digitalWrite(motorPinB, LOW);
              delay(onTime);
              digitalWrite(motorPinA, LOW);
              digitalWrite(motorPinB, LOW);
              delay(onTime);
              motorState = 1;
            }
            else{ //motorState == 1
              digitalWrite(motorPinA, LOW);
              digitalWrite(motorPinB, HIGH);
              delay(onTime);
              digitalWrite(motorPinA, LOW);
              digitalWrite(motorPinB, LOW);
              delay(onTime);
              motorState = 0;
            }
            //start counting to next beat only upon a valid beat
            firstBeatTime = secondBeatTime;
          }
      }
  }
  else{
    beating = false;
  }

  //prepare for edge detection on next cycle
  lastBeat = beating;
}

