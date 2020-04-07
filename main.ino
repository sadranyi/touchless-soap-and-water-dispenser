#include <Servo.h>

#define SOAP_SENSOR_TRIG_PIN 2
#define SOAP_SENSOR_ECHO_PIN 3

#define WATER_SENSOR_TRIG_PIN 4
#define WATER_SENSOR_ECHO_PIN 5

#define PAPER_SENSOR_TRIG_PIN 6
#define PAPER_SENSOR_ECHO_PIN 7

#define SOAP_PUMP_CONTROL_PIN 8
#define WATER_PUMP_CONTROL_PIN 9
#define PAPER_SERVO_PIN 10

#define BUZZER_PIN 11
#define RED_PIN 12
#define GREEN_PIN 13

Servo paperServo;

const int keepWaterOpen = 15000;
const int keepSoapOpen = 3000;
const int keepPaperRollingFor = 1500;

const int waterTriggerDistance = 3;
const int soapTriggerDistance = 3;
const int paperTriggerDistance = 2;

const int waitFor = 1000;
const int setupDelay = 100;
const int openAngel = 0;
const int closeAngel = 90;
const int beepDelay = 100;
const int paperSensorDelayAfterUse = 1500; // waits for user to tear paper
const long pulseTimeOut = 32760;

long waterSensorDuration;
long soapSensorDuration;
long paperSensorDuration;

float waterHandDistance;
float soapHandDistance;
float paperHandDistance;

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...");
  
  pinMode(WATER_SENSOR_TRIG_PIN, OUTPUT);
  pinMode(SOAP_SENSOR_TRIG_PIN, OUTPUT);
  pinMode(PAPER_SENSOR_TRIG_PIN, OUTPUT);

  pinMode(WATER_SENSOR_ECHO_PIN, INPUT);
  pinMode(SOAP_SENSOR_ECHO_PIN, INPUT);
  pinMode(PAPER_SENSOR_ECHO_PIN, INPUT);

  digitalWrite(WATER_SENSOR_TRIG_PIN, LOW);
  digitalWrite(SOAP_SENSOR_TRIG_PIN, LOW);
  digitalWrite(PAPER_SENSOR_TRIG_PIN, LOW);
  
  pinMode(WATER_PUMP_CONTROL_PIN, OUTPUT);
  digitalWrite(WATER_PUMP_CONTROL_PIN, LOW); 
  
  pinMode(SOAP_PUMP_CONTROL_PIN, OUTPUT);
  digitalWrite(SOAP_PUMP_CONTROL_PIN, LOW);
  
  paperServo.attach(PAPER_SERVO_PIN);
  paperServo.write(closeAngel);
  
  pinMode(BUZZER_PIN, OUTPUT);

  // Setup RGB LED
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  // set defaul color: 
  // GREEN(2) => Available 
  // RED(1) => for In-Use
  setColor(2);
  
  delay(setupDelay);
  Serial.println("Initialization Complete");
}

float getWaterDistance() {
  digitalWrite(WATER_SENSOR_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(WATER_SENSOR_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(WATER_SENSOR_TRIG_PIN, LOW);
  waterSensorDuration = pulseIn(WATER_SENSOR_ECHO_PIN, HIGH, pulseTimeOut);
  return waterSensorDuration * 0.0133/2;
}

float getSoapDistance() {
  digitalWrite(SOAP_SENSOR_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(SOAP_SENSOR_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SOAP_SENSOR_TRIG_PIN, LOW);
  soapHandDistance = pulseIn(SOAP_SENSOR_ECHO_PIN, HIGH, pulseTimeOut);
  return soapHandDistance * 0.0133/2;
}

float getPaperDistance() {
  digitalWrite(PAPER_SENSOR_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(PAPER_SENSOR_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(PAPER_SENSOR_TRIG_PIN, LOW);
  paperSensorDuration = pulseIn(PAPER_SENSOR_ECHO_PIN, HIGH, pulseTimeOut);
  return paperSensorDuration * 0.0133/2;
}

void beep(int delayFor, int beepCount){
    for (int i = 0; i < beepCount; i++){
      digitalWrite(BUZZER_PIN, HIGH);
      delay(delayFor);
      digitalWrite(BUZZER_PIN, LOW);
      delay(delayFor);
    }
}

// 1:RED, 2:GREEN
void setColor(int color){
  switch(color){
    case 1:
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
      break;
    case 2:
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      break;
    default:
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
  }
}

void loop() {
  soapHandDistance = getSoapDistance();
  waterHandDistance = getWaterDistance();
  paperHandDistance = getPaperDistance();

  if (soapHandDistance > 0 && soapHandDistance <= soapTriggerDistance) {
    beep(beepDelay, 1);
    Serial.println("Soap Dispensing Started ==>" );
    setColor(1);
    digitalWrite(SOAP_PUMP_CONTROL_PIN, HIGH);
    delay(keepSoapOpen);
    Serial.println("Soap Dispensing Stopped <==" );
    digitalWrite(SOAP_PUMP_CONTROL_PIN, LOW);
    setColor(2);
    beep(beepDelay, 2);
    delay(waitFor);
   }

   if (waterHandDistance > 0 && waterHandDistance <= waterTriggerDistance) {
    beep(beepDelay, 1);
    Serial.println("Water Dispensing Started ==>" );
    setColor(1);
    digitalWrite(WATER_PUMP_CONTROL_PIN, HIGH);
    delay(keepWaterOpen);
    Serial.println("Water Dispensing Stopped <==" );
    digitalWrite(WATER_PUMP_CONTROL_PIN, LOW);
    setColor(2);
    beep(beepDelay, 2);
    delay(waitFor);
   }

  if (paperHandDistance > 0  && paperHandDistance <= paperTriggerDistance) {
    beep(beepDelay, 1);
    Serial.println("Paper Dispensing Started ==>" );
    setColor(1);
    paperServo.write(openAngel);
    delay(keepPaperRollingFor);
    Serial.println("Paper Dispensing Stopped <==" );
    paperServo.write(closeAngel);
    setColor(2);
    beep(beepDelay, 2);
    delay(paperSensorDelayAfterUse);
   }
}