#include "Arduino.h"
#include <DHT.h>

const int Trigger_Pin = 2;
const int Echo_PIN = 3;
const int Trigger_Pin1 = 9;
const int Echo_PIN1 = 8;
int duration, distance1, distance2;

const int Buzzer_Pin = 4;
const int DHT_Pin = 5;
const int RedLED_Pin = 6;

int LED_State = LOW;
unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
const byte SLAVE_ADDRESS = 42;

DHT dht(5, DHT11);

void setup()
{
  Serial.begin(9600);                     //Default communication channel-

  //On Startup - Play Buzzer Tune indicating successful startup to the user.
  runStartupTune();

  dht.begin();

  pinMode(Trigger_Pin, OUTPUT);
  pinMode(Echo_PIN, INPUT);
  pinMode(Trigger_Pin1, OUTPUT);
  pinMode(Echo_PIN1, INPUT); 
  pinMode(Buzzer_Pin, OUTPUT);
  pinMode(RedLED_Pin, OUTPUT);
}

void loop()
{ 
  int Distance;
  int buzzerFreq;;
  int Temp;
  int Humidity;
  int Battery;

  //bluetoothReceive(minDetection, maxDetection, isMetric);

  distance1 = SonarSensor(Trigger_Pin, Echo_PIN);
  distance2 = SonarSensor(Trigger_Pin1, Echo_PIN1);
  
  Serial.print("Distance1: "); 
  Serial.print(distance1);

  Serial.print("         Distance2: "); 
  Serial.println(distance2);
  delay(500);
  
  collectDistance(Trigger_Pin, Echo_PIN, Buzzer_Pin, Distance, buzzerFreq);
  Humidity = dht.readHumidity();
  Temp = dht.readTemperature();

  //releaseBuzzer(Distance, buzzerFreq);
  /*
  Battery = collectBattery();
  if(Battery < 20)
    LED_Blink(500);
  else
    LED_State = HIGH;
  */
  
  Battery = 100;
/*
  if (millis() - previousMillis > 1500){      //run every 1.5seconds
    previousMillis = millis();
    
    Serial.println(Distance);

    if (millis() - previousMillis1 > 20000){   //run every 20seconds
      previousMillis1 = millis();
      
      //temp|humidity|battery  
      Serial.print(Temp);
      Serial.print("|");
      Serial.print(Humidity);  
      Serial.print("|");
      Serial.println(Battery);
    } 
  }*/
}

int SonarSensor(int trigPinSensor,int echoPinSensor)
{
  digitalWrite(trigPinSensor, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinSensor, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinSensor, LOW);

  duration = pulseIn(echoPinSensor, HIGH);
  return (duration/2) / 29.1;  
}

//http://www.martyncurrey.com/turning-a-led-on-and-off-with-an-arduino-bluetooth-and-android-part-ii-2-way-control/
//https://groups.google.com/g/mitappinventortest/c/F-9jyPUp72M/m/dAtp0TTcGgAJ
//delimiter
//https://groups.google.com/g/mitappinventortest/c/lVaCyAeWm_4/m/ADjvy05wAwAJ
//test with array of bytes - search "Kind of. I have nothing to test the code"
//https://community.appinventor.mit.edu/t/bluetooth-client-speed-and-buffering-issue/16064/18?page=2

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

int convertDistance(long duration, int modifier)
{
  /* modifier = 0
   * raw distance -> imperial
   * 
   * modifier = 1
   * raw distance -> metric
   */
  
  int distance;
  if(modifier == 0){
    distance = (duration/2) / 74;
  }
  else if(modifier == 1){
    distance = (duration * 0.034) / 2;
  }
  return distance;
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
  buzzerFreq = Distance * 5;
  if (Distance > maxDetect){
    //CASE1:
    noTone(Buzzer_Pin);                            
  }
  else if(Distance < minDetect){
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

void collectDistance(int, int, int, int &Distance, int &buzzerFreq) 
{
  /* measures duration the signal takes to traverse from TriggerPin to EchoPin
   * get distance from duration and convert to metric or imperial based on 
   * existing isMetric bool
   * isMetric = true      correlates to    [raw distance -> metric]
   * isMetric = false     correlates to    [raw distance -> imperial]
   */

  digitalWrite(Trigger_Pin, LOW);
  delayMicroseconds(2);
  digitalWrite(Trigger_Pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger_Pin, LOW);

  long Duration;
  Duration = pulseIn(Echo_PIN, HIGH);
  Distance = convertDistance(Duration, 0);
  
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
 /* causes the led to blink based on the global variable
  * unsigned long ledinterval = 500 found at the header of the file
  */
  if (millis() - previousMillis > frequency){
    previousMillis2 = millis();
    if (LED_State == LOW)
      LED_State = HIGH;
    else
      LED_State = LOW;
    digitalWrite(6, LED_State);
  }
}

//RSSI bullshit
//https://community.appinventor.mit.edu/t/help-me-with-implementing-a-few-formulas-that-i-need-in-my-arduino-code/20946/2
