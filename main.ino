#include <DHT.h>
#include "LowPower.h"

const int Trigger_Pin = 2;
const int Echo_PIN = 3;
const int Trigger_Pin1 = 9;
const int Echo_PIN1 = 8;
const int Buzzer_Pin = 4;
const int DHT_Pin = 5;
const int RedLED_Pin = 6;
const int TiltSwitch_Pin = 10;

int iBuzzerFreq;
float fDistance;
int iTemp;
int iHumidity;
int iBattery;

int LED_State = LOW;
unsigned long lgUpdateTime;
unsigned long lgInactivityTime;
unsigned long previousMillis = 0;

const long InternalReferenceVoltage = 1062;

DHT dht(5, DHT11);

void setup()
{
  Serial.begin(9600);

  //On Startup - Play Buzzer Tune indicating successful startup to the user.
  runStartupTune();

  dht.begin();

  pinMode(Trigger_Pin, OUTPUT);
  pinMode(Echo_PIN, INPUT);
  pinMode(Trigger_Pin1, OUTPUT);
  pinMode(Echo_PIN1, INPUT); 
  pinMode(Buzzer_Pin, OUTPUT);
  pinMode(RedLED_Pin, OUTPUT);
  pinMode(TiltSwitch_Pin, INPUT);
  pinMode(TiltSwitch_Pin, HIGH);
  
  lgUpdateTime = millis();
}

void loop()
{
  collectDistance(Trigger_Pin, Echo_PIN);
  iHumidity = int(dht.readHumidity());
  iTemp = int(dht.readTemperature());
  //releaseBuzzer(Distance, buzzerFreq);
      
      /*
      fBattery = collectBattery();
      if(fBattery < 20)
        LED_Blink(500);
      else
        LED_State = HIGH;
      */
      
      LED_Blink(500);
      iBattery = 100;
      if(millis() - lgUpdateTime > 1000) //Loop send approx every 1s
      {
           detectInactivity(300000);  //5mins/300000 of inactivity for lowPower mode to activate 
           lgUpdateTime = millis();
           
           Serial.print(fDistance/100, 2);
           Serial.print("|");  
           Serial.print(iTemp);
           Serial.print("|");
           Serial.print(iHumidity);  
           Serial.print("|");
           Serial.print(iBattery);
           Serial.println();
      }
}

void onWakeUp()
{
  //wake up tune from lowPower_Mode
  tone(Buzzer_Pin, 660, 100);
  delay(75);
  tone(Buzzer_Pin, 660, 100);
  delay(150);
  tone(Buzzer_Pin, 660, 100);
  delay(75);
  tone(Buzzer_Pin, 660, 100);
  delay(150);

  Serial.println("No more sleepy sleepy!!");

}

void detectInactivity(long inactivityDef)
{
  /* If Activity_Val == HIGH for duration of /Interval/, set bool isActive to false, check state of LP_Mode and trigger inside if needed 
   * Reset timer when Activity_Val == LOW, set bool isActive to true and check state of LP_Mode before enabling or disabling
   * Low Power Mode:
   * https://learn.sparkfun.com/tutorials/reducing-arduino-power-consumption/all
   * https://github.com/rocketscream/Low-Power/blob/master/LowPower.cpp
   */ 
   
  bool isActive;
  int activityValue = digitalRead(TiltSwitch_Pin);
  
  if(activityValue == LOW){
      
    isActive = true;
    lgInactivityTime = millis();
  }

  if(millis() - lgInactivityTime >= inactivityDef){
    isActive = false;
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
  }
}

float SonarSensor(int trigPinSensor, int echoPinSensor)
{
  //40ms delay on the rotation between the two UltraSensors
  //edge case where two Ultrasonics can mess up distance collection reliability
  
  digitalWrite(trigPinSensor, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinSensor, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinSensor, LOW);

  int duration = pulseIn(echoPinSensor, HIGH);
  return (duration/2) / 29.1; 
}



void collectDistance(int, int) 
{
  /* measures duration the signal takes to traverse from TriggerPin to EchoPin
   * this operation is calculated from two seperate Ultrasonic Sensors of which the lower value 
   * is picked and determined to be the distance that's sent out to the App
   */
   
  float distance1 = SonarSensor(Trigger_Pin, Echo_PIN);
  float distance2 = SonarSensor(Trigger_Pin1, Echo_PIN1);

  Serial.print(distance1);
  Serial.print(" ");
  Serial.println(distance2);
  delay(1000);

  if(distance1 > distance2)
    fDistance = distance2;
  else
    fDistance = distance1;
 }

int collectBattery()
{
  //reads battery voltage and returns battery value int in a range of 0-100
  int value = 0;
  float voltage;
  float perc;

    value = analogRead(A0);
  voltage = value * 3.2/1023;
     perc = map(voltage, 3.6, 4.2, 0, 100);
  return perc;
}

void releaseBuzzer(int Distance, int buzzerFreq)
{
  /*  Buzzer warning when an object is within maxDetection range of the device
   *  REFERENCE CODE: tone(pin, frequency, duration)
   *  CASE 1: no buzzer beeps when distance above max detection 
   *    distance > maxDetection
   *    noTone(Buzzer_Pin);
   *    
   *  CASE 2: continuous buzzer beeps when distance lower than min detection
   *    distance < minDetection -> 
   *    tone(Buzzer_Pin, 1800, buzzerFreq);
   *
   *  CASE 3: beep based on buzzerFrequency when distance between max detection and min detection
   *    minDetection < distance < maxDetection
   *    tone(Buzzer_Pin, 1800, buzzerFreq); delay(buzzerFreq);
   */ 
    
  int minDetect = 5;
  int maxDetect = 20;
  
  //***Buzzer Frequency Controller***
  buzzerFreq = fDistance * 5;
  if (fDistance > maxDetect){
    //CASE1:
    noTone(Buzzer_Pin);                            
  }
  else if(fDistance < minDetect){
    //CASE2:
    buzzerFreq = 0;
    tone(Buzzer_Pin, 1800, buzzerFreq);
  }
  else{
    //CASE3:
    tone(Buzzer_Pin, 1800, buzzerFreq);
    delay(buzzerFreq);
  }
}

void runStartupTune()
{
  /* to be used as indicator to the user that
   * the device succesfully booted
   */
  tone(Buzzer_Pin, 660, 100);
  delay(75);
  tone(Buzzer_Pin, 660, 100);
  delay(150);
  tone(Buzzer_Pin, 660, 100);
  delay(75);
  tone(Buzzer_Pin, 660, 100);
  delay(500);   
}

void LED_Blink(int frequency)
{
 /* the famous blink without delay Arduino example
  * takes frequency of the led blink as parameter
  */
  
  if (millis() - previousMillis > frequency){
    previousMillis = millis();
    if (LED_State == LOW)
      LED_State = HIGH;
    else
      LED_State = LOW;
    digitalWrite(6, LED_State);
  }
}

//RSSI bullshit
//https://community.appinventor.mit.edu/t/arduino-distance-meassuring-through-bluetooth-classic-signal-strength-rssi-project/21175
//https://community.appinventor.mit.edu/t/help-me-with-implementing-a-few-formulas-that-i-need-in-my-arduino-code/20946/2
