#include <Servo.h> 

#define myFish 9
#define sensorPin A0
#define numSensorReadings 10

Servo fishServo;

int fishDirection = 45;
int lightLimit = 800;

unsigned long prevTimeFish;
unsigned long prevTimeWater;
unsigned long prevTimeSensor;

unsigned int fishDelay = 200;
unsigned int waterDelay = 50;
unsigned int sensorDelay = 2;

uint8_t waterPosition = 0;
uint8_t portValue = 0;

int sensorReadings[numSensorReadings];// the readings from the light sensor
int sensorReadIndex = 0;              // the index of the current reading
int sensorTotal = 0;                  // the running total
int sensorAverage = 0;                // the average

void setup() {
  
  // initialize port for water lights
  for(uint8_t id = 0; id < 8 ; id++){
   pinMode(id, OUTPUT); 
  }
  
  //Serial.begin(9600);
  
  // initializes servo
  fishServo.attach(myFish);
  fishServo.write(fishDirection);
  
  // initializes timers
  prevTimeFish = millis();
  prevTimeWater = millis();
  prevTimeSensor = millis();
  
  // initializing values for sensor readings, setting initial value to 0
  for (int thisReading = 0; thisReading < numSensorReadings; thisReading++)
    sensorReadings[thisReading] = 0;
}

void loop() {
  
  // gather the current time
  unsigned long curMS = millis();
  
  // make lights move! so pretty! :)
  if(curMS - prevTimeWater > waterDelay){
      if(waterPosition == 0){
        portValue = B11000000;      // start with the 2 first ones on
      }else if(waterPosition == 7){
        portValue = B10000001;      // at the end, shift the light to the beginning.
      }else{
        portValue = portValue >> 1; //slide the ON lights
      }  
      waterPosition ++;            // change to the next position for the next time that this runs!
      if(waterPosition == 8){
       waterPosition = 0;          // never let it go over 7, because we only have 8 leds.
      }
    prevTimeWater = curMS;         // store the current time, to use it next time the loop runs.
  }
  // set the lights on! 
  PORTD = portValue;
  
  // gather info from the sensor only every 2 ms
  if(curMS - prevTimeSensor > sensorDelay){ 
    sensorTotal = sensorTotal - sensorReadings[sensorReadIndex];
    sensorReadings[sensorReadIndex] = analogRead(sensorPin);
    sensorTotal = sensorTotal + sensorReadings[sensorReadIndex];
    sensorReadIndex++;
  
    if (sensorReadIndex >= numSensorReadings)
      sensorReadIndex = 0;
  
    sensorAverage = sensorTotal / numSensorReadings;
    //Serial.println(sensorAverage);
    
    // IF there is a lot of light, it means that the fish IS NOT in the water
    if (sensorAverage > lightLimit){
      // since it's NOT IN the water, we want to make it "jittery" (stressed)
      fishDelay = 8;
    }
    // IF there is not a lot of light, it means that the fish IS in the water
    else{
      // since it's IN the water, it is swimming happily
      fishDelay = 200;
    }
    
    prevTimeSensor = curMS;     // store the current time, to use it next time the loop runs.
  }
  
  // this loop only runs as often as the light sensor tells it to run. Very often when there is a lot of light (fish not in water),
  // not so often when there is not much light (fish is in the water)
  
  if(curMS - prevTimeFish > fishDelay){
    // if the fish is going towards 45 when the loop runs, SWITCH to 135 (go CW)
    if(fishDirection == 45){
      fishDirection = 135;
    }
    // if the fish is going towards 135 when the loop runs, SWITCH to 45 (go CCW)
    else{
      fishDirection = 45;
    }
    // actually tell the fish to move!
    fishServo.write(fishDirection);
    prevTimeFish = curMS; // store the current time, to use it next time the loop runs.
  }

}

