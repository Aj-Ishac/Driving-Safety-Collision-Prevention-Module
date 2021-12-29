#include "Arduino.h"
#include <SimpleDHT.h>
#include <Wire.h>
#include <I2C_Anything.h>

const int Trigger_Pin = 3;
const int Echo_PIN = 2;
const int Buzzer_Pin = 4;
const int DHT_Pin = 5;
const int RedLED_Pin = 6;

int LED_State = LOW;
unsigned long previousMillis = 0;
unsigned long ledinterval = 500;             //***LED BLINK FREQUENCY CONTROLLER***
const byte SLAVE_ADDRESS = 42;

SimpleDHT11 dht11(DHT_Pin);

void setup()
{

  Serial.begin(9600);                     //Default communication channel
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
  float HCSR04_Distance;
  int   Buzzer_Frequency;
  bool  isBeeping;
  byte DHT11_Temperature;
  byte DHT11_Humidity;
  
  Ultrasonic_CollisionDetect(Trigger_Pin, Echo_PIN, Buzzer_Pin, HCSR04_Distance, Buzzer_Frequency, isBeeping);
  TempHumidity_Detect(DHT_Pin,DHT11_Temperature,DHT11_Humidity);

  //add Buzzer_Frequency to led blink frequency parameter
  LED_Blink(RedLED_Pin);                    


  Wire.beginTransmission (SLAVE_ADDRESS);
  I2C_writeAnything (HCSR04_Distance);
  I2C_writeAnything (Buzzer_Frequency);
  I2C_writeAnything (isBeeping);
  I2C_writeAnything (DHT11_Temperature);
  I2C_writeAnything (DHT11_Humidity);
  Wire.endTransmission ();
 
  delay(250);
}

void Ultrasonic_CollisionDetect(int, int, int,float &HCSR04_Distance,int &Buzzer_Frequency, bool &isBeeping) 
{
  //need to define HCSR04_Distance, Buzzer_Frequency, isBeeping
  digitalWrite(Trigger_Pin, LOW);
  delayMicroseconds(2);
  digitalWrite(Trigger_Pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger_Pin, LOW);

  int Min_Detection = 20;                  //***Min Detection Controller***  current: 0.4m  CURRENT VALUES ARE FOR TESTING PURPOSES
  int Max_Detection = 55;                 //***Max Detection Controller***  current: 3.0m  CURRENT VALUES ARE FOR TESTING PURPOSES

  long Duration;                          //Duration of signal to leave TriggerPin and return to EchoPin
  float Distance;                         //Calculated distance in centimeters
  int BuzzerFreq;
  //bool isBeeping;
  
  Duration = pulseIn(Echo_PIN, HIGH);     //Duration signal takes to traverse from TriggerPin to EchoPin
  Distance = (Duration * 0.034) / 2;      //Distance calculation in cm - currently linear. needs to be exponential for satisfying feedback
  HCSR04_Distance = Distance;
   
  BuzzerFreq = Distance * 5;             //***Buzzer Frequency Controller** current base value: Distance * 10;
  
  
  
  Buzzer_Frequency = BuzzerFreq;

  //Turn off buzzer when higher than min distance
  //noTone(Buzzer_Pin);  turns off sound
  if (Distance > Max_Detection){          
    noTone(Buzzer_Pin);                            
    isBeeping = false;
    Serial.print("Track 2 ");
    Serial.print("BuzzerFreq: ");
    Serial.print(BuzzerFreq);
    Serial.print(" ");
  }
    else
  {
    //Continuously buzzer beep max frequency when lower than min distance
    if (Distance < Min_Detection){         
      BuzzerFreq = 0;
      tone(Buzzer_Pin, 1800, BuzzerFreq);
      isBeeping = false;
      Serial.print("Track 0 ");
      Serial.print("BuzzerFreq: ");
      Serial.print(BuzzerFreq);
      Serial.print(" ");
    }
      else
    { //If distance is between min and max distance, frequency of buzzing dependent on BuzzerFreq/Distance
      //tone(Buzzer_Pin, 1800, 250) ex:tone(Buzzer_Pin, desired frequency Hz, duration in ms)
      tone(Buzzer_Pin, 1800, BuzzerFreq);
      delay(BuzzerFreq);
      //noTone(Buzzer_Pin);
      isBeeping = true;
      Serial.print("Track 1 ");
      Serial.print("BuzzerFreq: ");
      Serial.print(BuzzerFreq);
      Serial.print(" ");

    }
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

//beeping_freq needs to be more exponential vs linear slope
//beeping freq rework: needs to be independant of exact value of min/max and leaning more towards how close and how far are we to min/max    
//blink when special state: true - figure out wtf special state will be used for later**********************************************************************************

//***main device***
//ulrasonic work HC-SR04        DONE
//buzzer Piezzo Buzzer          DONE
//humidity and temp DHT11       DONE
//led                           DONE
//bluetooth HC-05               NEXT
//button
//battery                       
