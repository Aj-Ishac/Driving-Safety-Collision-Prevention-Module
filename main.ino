#include "Arduino.h"
#include <SimpleDHT.h>
#include <SoftwareSerial.h>

const int Trigger_Pin = 2;
const int Echo_PIN = 3;
const int Buzzer_Pin = 4;
const int DHT_Pin = 5;
const int RedLED_Pin = 6;

int LED_State = LOW;
unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0;
unsigned long ledinterval = 500;          //***LED BLINK FREQUENCY CONTROLLER***
const byte SLAVE_ADDRESS = 42;

SimpleDHT11 dht11(DHT_Pin);

void setup()
{
  Serial.begin(9600);                     //Default communication channel-

  //On Startup - Play Buzzer Tune indicating successful startup to the user.
  runStartupTune();  

  pinMode(Trigger_Pin, OUTPUT);
  pinMode(Echo_PIN, INPUT);
  pinMode(Buzzer_Pin, OUTPUT);
  pinMode(RedLED_Pin, OUTPUT);
}

void loop()
{ 
  int Distance;
  int   buzzerFreq;
  byte DHT11_Temperature;
  byte DHT11_Humidity;
  int Battery;
  
  int minDetection = 0;
  int maxDetection = 0;
  bool isMetric;

  bluetoothReceive(minDetection, maxDetection, isMetric);
  collectDistance(Trigger_Pin, Echo_PIN, Buzzer_Pin, Distance, buzzerFreq, isMetric);
  //releaseBuzzer(Distance, buzzerFreq, minDetection, maxDetection);

  //WIP: add cooldown on collecting new temp/humidity/battery values
  //collectTempHumdity(DHT_Pin, DHT11_Temperature, DHT11_Humidity);
  
  /*
  Battery = collectBattery();
  if(Battery < 20)
    LED_Blink(RedLED_Pin);
  else
    LED_State = HIGH;
  */
  Battery = 100;

  if (millis() - previousMillis > 1500){    //run every 1.5seconds
    
    previousMillis = millis();
    Serial.println(Distance);

    if (millis() - previousMillis1 > 10000){   //run every 20seconds

      previousMillis1 = millis();
      //temp|humidity|battery
      Serial.print(DHT11_Temperature);
      Serial.print("|");
      Serial.print(DHT11_Humidity);  
      Serial.print("|");
      Serial.println(Battery);
    
    } 
  }
}

//http://www.martyncurrey.com/turning-a-led-on-and-off-with-an-arduino-bluetooth-and-android-part-ii-2-way-control/
//https://groups.google.com/g/mitappinventortest/c/F-9jyPUp72M/m/dAtp0TTcGgAJ
//delimiter
//https://groups.google.com/g/mitappinventortest/c/lVaCyAeWm_4/m/ADjvy05wAwAJ
//test with array of bytes - search Kind of. I have nothing to test the code
//https://community.appinventor.mit.edu/t/bluetooth-client-speed-and-buffering-issue/16064/18?page=2


void bluetoothReceive(int &minDetection, int &maxDetection, bool &isMetric)
{
  /* BT.read() from app user input, only occurs when user presses the button responsible:
   * minDetection, maxDetection, metricConvert, imperialConvert
   * 
   * user inputs: minDetection, maxDetection
   * modifies max/min Detection based on user preferences
   * 
   * user inputs: metric convert button
   * BTreceives = "mC"
   * bool isMetric = true, triggers metricConvert function on loop from now on
   * 
   * user inputs: imperial convert button
   * BTreceives = "iC"
   * bool isMetric = off, triggers imperialConvert function on loop from now on
   */
  
  String BTreceive;
  if(Serial.available()){
    BTreceive = Serial.read();
    Serial.print("BT RECEIVE: ");
    Serial.println(BTreceive);
    
    if(BTreceive == "mC"){
      isMetric = true;
      return;
    }  
    if(BTreceive == "iC"){
      isMetric = false;
      return;
    }
    int index = BTreceive.indexOf("-");
    minDetection = (BTreceive.substring(0, index)).toInt();
    maxDetection = (BTreceive.substring(index + 1)).toInt();
  } 
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

void releaseBuzzer(int Distance, int buzzerFreq, int minDetection, int maxDetection)
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

  int minDetect = minDetection;
  int maxDetect = maxDetection;
  
  //default value
  //0 value comparison needs work
  if(minDetect == 0 && maxDetect == 0)
  {
    minDetect = 5;
    maxDetect = 20;
  }

  //***Buzzer Frequency Controller***
  buzzerFreq = Distance * 5;
  if (Distance > maxDetection){
    //CASE1:
    noTone(Buzzer_Pin);                            
  }
  else if(Distance < minDetection){
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

void collectDistance(int, int, int, int &Distance, int &buzzerFreq, bool &isMetric) 
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
  
  if(isMetric = true)
    Distance = convertDistance(Duration, 0);
  else
    Distance = convertDistance(Duration, 1);
}
 
void collectTempHumdity(int, byte &DHT11_Temperature, byte &DHT11_Humidity)
{
    //collect temp and humidity facilitated by the SimpleDHT library
    byte temperature = 0;
    byte humidity = 0;
  
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess){
    Serial.print("Read DHT11 failed, err=");
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(",");
    Serial.println(SimpleDHTErrDuration(err));
    
    delay(200);
    return;
    }

    DHT11_Temperature = temperature;
    DHT11_Humidity = humidity;
    delay(200); 
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

void LED_Blink(int LED_Input)
{
 /* causes the led to blink based on the global variable
  * unsigned long ledinterval = 500 found at the header of the file
  */
  unsigned long currentMillis = millis(); 
  if (currentMillis - previousMillis > ledinterval){
    previousMillis = currentMillis;
    if (LED_State == LOW)
      LED_State = HIGH;
    else
      LED_State = LOW;
    digitalWrite(LED_Input, LED_State);
  }
}

//RSSI bullshit
//https://community.appinventor.mit.edu/t/help-me-with-implementing-a-few-formulas-that-i-need-in-my-arduino-code/20946/2
