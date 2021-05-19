/*
  Based on Tutorial: Control Stepper Motor With Blynk App Using ESP32

  gbecki - May 2021
  
  Board:
  - Nodemcu
  
  Output:
  - 5V 28BYJ-48 Stepper Motor
    https://my.cytron.io/c-motor-and-motor-driver/c-dc-motor/c-stepper-motor/p-12v-28byj-48-stepper-motor-plus-uln2003-driver-board

  Library Manager:
  - Blynk by Volodymyr Shymanskyy Version 0.6.1
*/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <AccelStepper.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";

#define motorPin1 5 // IN1 pin on the ULN2003A driver to pin D1 on NodeMCU board
#define motorPin2 4 // IN2 pin on the ULN2003A driver to pin D2 on NodeMCU board
#define motorPin3 0 // IN3 pin on the ULN2003A driver to pin D3 on NodeMCU board
#define motorPin4 2 // IN4 pin on the ULN2003A driver to pin D4 on NodeMCU board

AccelStepper stepper(AccelStepper::HALF4WIRE, motorPin4, motorPin2, motorPin3, motorPin1);

#define STEP_ANGLE 11.3777778
#define BLYNK_INTERVAL 107.2
uint32_t lastUpdate = 0;

int move;
int zeroPos;
float storPos;

BLYNK_CONNECTED() 
{
  Blynk.syncVirtual(V4);
}

BLYNK_WRITE(V1) // Button Widget is writing to pin V1 (CCW rotation)
{
  move = param.asInt();
  if (move == 0) {
    stepper.setSpeed(0);
    stepper.disableOutputs();  
  }    
  if (move == 1) {
    stepper.enableOutputs();
    stepper.setMaxSpeed(1000.0);
    stepper.setSpeed(-100);
    stepper.move(1000);
  }
}

BLYNK_WRITE(V2) // Button Widget is writing to pin V2 (CW rotation)
{
  move = param.asInt();
  if (move == 0) {
    stepper.setSpeed(0);
    stepper.disableOutputs();  
  }    
  if (move == 1) {
    stepper.enableOutputs();
    stepper.setMaxSpeed(1000.0);
    stepper.setSpeed(100);
    stepper.move(1000);
  }
}

BLYNK_WRITE(V3) // Button Widget is writing to pin V3
{
  zeroPos = param.asInt();
  if (zeroPos == 1) {
    stepper.setCurrentPosition(0); // Zero the position if needed
  }
}

BLYNK_WRITE(V4)
{
  storPos = param.asFloat(); // Retrieve the data from server
  stepper.setCurrentPosition(storPos * STEP_ANGLE);
}

BLYNK_READ(V4) // Gauge to receive current Azimuth
{
  Blynk.virtualWrite(V4, (stepper.currentPosition() / STEP_ANGLE)); //Blynk 2 decimal places format: /pin.##/
}

void setup()
{
  Serial.begin(115200);
  Serial.print("Initialize Blynk.");

  Blynk.begin(auth, ssid, pass);
 
  stepper.setMaxSpeed(0.0); // set the max motor speed
  stepper.setAcceleration(0.0); // set the acceleration
  stepper.setSpeed(0.0); // set the current speed
}

void loop() 
{  
  if (millis() - lastUpdate > BLYNK_INTERVAL) {
    lastUpdate = millis();
    Blynk.run();      
  }
  stepper.runSpeed();
}

/*
* Converts degrees to steps
*
* 28BYJ-48 motor has 5.625 degrees per step (half step)
* 360 degrees / 5.625 = 64 steps per revolution
*
* Example with 45 deg:
* (64 / 5.625) * 45 = 512 steps
*/
