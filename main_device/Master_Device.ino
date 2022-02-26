#include "Arduino.h"
#include <SimpleDHT.h>
#include <Wire.h>
#include <I2C_Anything.h>
#include <SoftwareSerial.h>

const int Trigger_Pin = 3;
const int Echo_PIN = 2;
const int Buzzer_Pin = 4;
const int DHT_Pin = 5;
const int RedLED_Pin = 6;

int LED_State = LOW;
unsigned long previousMillis = 0;
unsigned long ledinterval = 500;          //***LED BLINK FREQUENCY CONTROLLER***
const byte SLAVE_ADDRESS = 42;

SimpleDHT11 dht11(DHT_Pin);
SoftwareSerial BT(10,11);                 //TX,RX pins respectively

void setup()
{

  Serial.begin(9600);                     //Default communication channel
  BT.begin(9600);                         //Initiate BT communication
  Serial.println("Master Serial channel is ready at 9600");
  Wire.begin();
  
  delay(1000);

  pinMode(Trigger_Pin, OUTPUT);
  pinMode(Echo_PIN, INPUT);
  pinMode(Buzzer_Pin, OUTPUT);
  pinMode(RedLED_Pin, OUTPUT);
  
}


void loop()
{ 
  float Distance;
  int   BuzzerFreq;
  bool  isBeeping;
  byte DHT11_Temperature;
  byte DHT11_Humidity;
  int Battery;

  String BTData;

  Ultrasonic_CollisionDetect(Trigger_Pin, Echo_PIN, Buzzer_Pin, Distance, BuzzerFreq, isBeeping);
  TempHumidity_Detect(DHT_Pin,DHT11_Temperature,DHT11_Humidity);

  //add Buzzer_Frequency to led blink frequency parameter
  LED_Blink(RedLED_Pin);                    

  //temp wire data transfer from master to slave device
  Wire.beginTransmission (SLAVE_ADDRESS);
  I2C_writeAnything (Distance);
  I2C_writeAnything (BuzzerFreq);
  I2C_writeAnything (isBeeping);
  I2C_writeAnything (DHT11_Temperature);
  I2C_writeAnything (DHT11_Humidity);
  Wire.endTransmission ();

  //distance;temp;humidity;battery string bluetooth send
  BTData = String(Distance) + ";" + String(DHT11_Temperature) + ";" + String(DHT11_Humidity) + ";" + String(Battery); 
  Serial.print(BTData);
  BT.println(BTData);
  
  delay(400);
}

void Ultrasonic_CollisionDetect(int, int, int,float &Distance,int &BuzzerFreq, bool &isBeeping) 
{
  digitalWrite(Trigger_Pin, LOW);
  delayMicroseconds(2);
  digitalWrite(Trigger_Pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger_Pin, LOW);

  //IF CHANGED, MAKE SURE TO REFLECT CHANGES IN void BuzzerBeep IN SLAVE_DEVICE.CPP
  int Min_Detection = 20;                 //***Min Detection Controller***
  int Max_Detection = 55;                 //***Max Detection Controller***

  long Duration;                          
  
  Duration = pulseIn(Echo_PIN, HIGH);     //Duration signal takes to traverse from TriggerPin to EchoPin
  Distance = (Duration * 0.034) / 2;      //Distance calculation in cm - currently linear. needs to be exponential for satisfying feedback 
  BuzzerFreq = Distance * 5;              //***Buzzer Frequency Controller** current base value: Distance * 10;
  
  //no buzzer beeps when !distance above max detection!
  if (Distance > Max_Detection){          
    noTone(Buzzer_Pin);                            
    isBeeping = false;
    Serial.print("Track 2 ");
    Serial.print("BuzzerFreq: ");
    Serial.print(BuzzerFreq);
    Serial.print(" ");
  }
  //continuous buzzer beeps when !distance lower than min detection!
  else if(Distance < Min_Detection)
  {
    BuzzerFreq = 0;
    tone(Buzzer_Pin, 1800, BuzzerFreq);
    isBeeping = false;
    Serial.print("Track 0 ");
    Serial.print("BuzzerFreq: ");
    Serial.print(BuzzerFreq);
    Serial.print(" ");
  }
  //buzzer beep based on buzzerfrequency when !distance between max detection and min detection!
  //tone(Buzzer_Pin, 1800, 250) ex:tone(Buzzer_Pin, desired frequency Hz, duration in ms)  
  else
    {
      tone(Buzzer_Pin, 1800, BuzzerFreq);
      delay(BuzzerFreq);
      //noTone(Buzzer_Pin);
      isBeeping = true;
      Serial.print("Track 1 ");
      Serial.print("BuzzerFreq: ");
      Serial.print(BuzzerFreq);
      Serial.print(" ");
    }

  Serial.print("Distance:");
  Serial.print(Distance);
  Serial.print("cm");
  Serial.print(" isBeeping:");
  Serial.println(isBeeping);
  
  delay(250);
}
  
void TempHumidity_Detect(int, byte &DHT11_Temperature, byte &DHT11_Humidity)
{
    byte temperature = 0;
    byte humidity = 0;
  
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
    {
    Serial.print("Read DHT11 failed, err=");
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(",");
    Serial.println(SimpleDHTErrDuration(err));

    //temp = temperature;
    //hum  = humidity;
    
    delay(200);
    
    return;
    }

    Serial.print("Temp: ");
    Serial.print((int)temperature);
    DHT11_Temperature = temperature;
    Serial.print(" *C, ");
    Serial.print((int)humidity);
    DHT11_Humidity = humidity;
    Serial.println(" H");

    delay(200);
    
}

void LED_Blink(int LED_Input)
{
  unsigned long currentMillis = millis();
   
  if (currentMillis - previousMillis > ledinterval) {
    previousMillis = currentMillis;
    if (LED_State == LOW)
      LED_State = HIGH;
    else
      LED_State = LOW;
    digitalWrite(LED_Input, LED_State);
  }
}



    //21:27:10.112 -> OK
    //21:27:39.837 -> +UART:4800,0,0,
    //21:27:39.837 -> OK
    //21:27:45.474 -> OK
    //21:27:53.807 -> OK
    //21:28:44.808 -> OK

//main:
//main device will send and receive BT data
//buzzer beep needs to be %based of distance so beep frequency is unaffected by changing min/max detection values
//receive BT data of min/max detection value changes and apply here, and onto slave device on the next slave startup
//battery% to implement low battery warning in LED blinking feedback, and for BT data sent to phone for data relay
//button/switch to turn power off

//slave:
//slave device will only receive BT data
//on startup: check if min/max detection values have been changed since last use, adjust if so
//buzzer beep needs to be %based of distance
//button press to enable idle/low battery mode -> turn off lcd backlight till next normal press
//button/switch to turn power off

//android:
//android app will send and receive BT data
//screen 0, establish BT connection -> brief intro goes here
//screen 1, data relay -> BT RSSI chart
//                     -> Receive Battery% and map it to block value
//                     -> Overlay: Warning when distance is within alert range
//                     -> Warning Method: Ring and/or Vibrate, or Mute
//screen 2, Options    -> Static Page: Explain data points relayed
//screen 3, Diags      -> Yet to implement, log console goes here
//screen 4, Configs    -> Alerts: Range,
//                     -> Unit Convert Button, change Distance+m to Distance+ft/inches to notify user
//                     -> if in Imperial units, change to metric in the backend before BT send to main device
//                     -> BT send new min/max detection values to main device
//                     -> Alerts: Preference,
//                     -> Vibrate and/or Ring button, or Mute button with reconfirm warning before applying Mute
//                     -> Log console: Notify user of changes made                       

//wrap up:
//documentation
//housing of main and slave device
//magnet


//BT
//THIS IS WHAT WORKS FOR MASTER
//http://phillipecantin.blogspot.com/2014/08/hc-05-bluetooth-link-with-zero-code.html
//https://www.youtube.com/watch?v=hyME1osgr7s
//https://howtomechatronics.com/tutorials/arduino/how-to-configure-pair-two-hc-05-bluetooth-module-master-slave-commands/
//http://www.martyncurrey.com/connecting-2-arduinos-by-bluetooth-using-a-hc-05-and-a-hc-06-pair-bind-and-link/

//https://www.instructables.com/How-to-Configure-HC-05-Bluetooth-Module-As-Master-/
//POTENTIAL FIX TO BT SHITSHOW

//TRANSFER DATA THROUGH ARDUINOS THROUGH WIRES
//www.youtube.com/watch?v=PnG4fO5_vU4         
