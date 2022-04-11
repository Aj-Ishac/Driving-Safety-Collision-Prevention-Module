#include <DHT.h>

const int Trigger_Pin = 2;
const int Echo_PIN = 3;
const int Trigger_Pin1 = 9;
const int Echo_PIN1 = 8;
const int Buzzer_Pin = 4;
const int DHT_Pin = 5;
const int RedLED_Pin = 6;
const int TiltSwitch_Pin = 10;

int iBuzzerFreq;
float fDistance, distance1, distance2;
int iTemp;
int iHumidity;
int iBattery;
int duration; 

int LED_State = LOW;
unsigned long lgUpdateTime;
unsigned long previousMillis = 0;

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
      
      //LED_Blink(500);
      iBattery = 100;
    if(millis() - lgUpdateTime > 1000) //Loop send approx every 1.5 seconds
      {
           lgUpdateTime = millis();
           isIdle();
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

//http://www.martyncurrey.com/turning-a-led-on-and-off-with-an-arduino-bluetooth-and-android-part-ii-2-way-control/
//https://groups.google.com/g/mitappinventortest/c/F-9jyPUp72M/m/dAtp0TTcGgAJ
//delimiter
//https://groups.google.com/g/mitappinventortest/c/lVaCyAeWm_4/m/ADjvy05wAwAJ
//test with array of bytes - search "Kind of. I have nothing to test the code"
//https://community.appinventor.mit.edu/t/bluetooth-client-speed-and-buffering-issue/16064/18?page=2

void isIdle()
{
  int Activity_Val = digitalRead(TiltSwitch_Pin);
  if (Activity_Val == HIGH)
    Serial.println("HIGH");
  else
    Serial.println("LOW");
}

float SonarSensor(int trigPinSensor, int echoPinSensor)
{
  digitalWrite(trigPinSensor, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinSensor, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinSensor, LOW);

  duration = pulseIn(echoPinSensor, HIGH);
  return (duration/2) / 29.1;  
}

void collectDistance(int, int) 
{
  /* measures duration the signal takes to traverse from TriggerPin to EchoPin
   * this operation is calculated from two seperate Ultrasonic Sensors of which the lower value 
   * is picked and determined to be the distance that's sent out to the App
   */
   
  distance1 = SonarSensor(Trigger_Pin, Echo_PIN);
  distance2 = SonarSensor(Trigger_Pin1, Echo_PIN1);

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
//https://community.appinventor.mit.edu/t/help-me-with-implementing-a-few-formulas-that-i-need-in-my-arduino-code/20946/2
