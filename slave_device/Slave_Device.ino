//---------------LIBRARIES---------------
#include "Arduino.h"
#include <Wire.h> 
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <I2C_Anything.h>
#include <SoftwareSerial.h>

//---------------CONSTANTS---------------
const int GreenLED_Pin = 2;
const int Buzzer_Pin = 3;
const byte MY_ADDRESS = 42;

volatile boolean haveData = false;
volatile float   HCSR04_Distance;
volatile int     Buzzer_Frequency;
volatile bool    isBeeping = false;
volatile byte    DHT11_Temperature;
volatile byte    DHT11_Humidity;

//SDA = A4;
//SCL = A5;
//A4 -> GREEN -> GREEN -> GREEN -> A4 
//A5 -> YELLOW -> YELLOW -> BROWN -> A5

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7);
SoftwareSerial BT(10,11);                 //TX,RX pins respectively

//---------------------------------------
 
void setup()
{
  Serial.begin(9600);
  Serial.println("Slave Serial channel is ready at 9600");
  
  pinMode(GreenLED_Pin, OUTPUT);
  Wire.begin (MY_ADDRESS);
  Wire.onReceive (receiveEvent);
  
  delay(500);

  digitalWrite(GreenLED_Pin, HIGH);       //if powered -> green led on
  
  lcd.begin(20,4); 
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);

  Serial.println("*******************************************");
  
}

//todo
//buzzer + buzzer freq -> green led
//temp wired value transfer
//lcd

//---------------------------------------

void loop()
{
    //temp measures for wired data transfer
    if (haveData)
    {
    SerialPrintReceivables(HCSR04_Distance,Buzzer_Frequency,isBeeping,DHT11_Temperature,DHT11_Humidity);
    LCDPrintReceivables(HCSR04_Distance, Buzzer_Frequency, isBeeping, DHT11_Temperature, DHT11_Humidity);
    //LCD_CustomSymbol();
    
    haveData = false;
    }

    if(Serial.available() > 0){

       Incoming_value = Serial.read();
       Serial.println(Incoming_value);

      //parse incoming_value by splitting Incoming_value in two seperated by ;
      //by doing so we get following values:
      //Distance, DHT11_Temperature, DHT11_Humidity, Battery1, Min_Detection, Max_Detection
  
    }
    
  delay(250);

}

void BuzzerBeep(Buzzer_Frequency)
{   
    //REPLICATE BEEPS FROM MAIN DEVICE ON SLAVE DEVICE
    //TAKES IN BUZZER_FREQ FROM MAIN DEVICE
    //NEEDS TESTING
      
    //IF CHANGED, MAKE SURE TO REFLECT CHANGES IN void Ultrasonic_CollisionDetect IN MAIN_DEVICE.CPP
    int Min_Detection = 20;
    int Max_Detection = 55;
    
    //Turn off buzzer when higher than min distance
    if (Distance > Max_Detection){          
      noTone(Buzzer_Pin);                            
      isBeeping = false;
      Serial.print("Track 2 ");
      Serial.print("BuzzerFreq: ");
      Serial.print(BuzzerFreq);
      Serial.print(" ");
      
  }if else(Distance < Min_Detection){     
      BuzzerFreq = 0;
      tone(Buzzer_Pin, 1800, BuzzerFreq);
      isBeeping = false;
      Serial.print("Track 0 ");
      Serial.print("BuzzerFreq: ");
      Serial.print(BuzzerFreq);
      Serial.print(" ");
    }
      else
    {
      tone(Buzzer_Pin, 1800, BuzzerFreq);
      delay(BuzzerFreq);
      isBeeping = true;
      Serial.print("Track 1 ");
      Serial.print("BuzzerFreq: ");
      Serial.print(BuzzerFreq);
      Serial.print(" ");
    }
}

void Convert_Meters_Or_AboveMax(float)
{
  if(HCSR04_Distance > 399)
    {
      lcd.print("Distance: ");
      lcd.print(">4m");
      lcd.print("    ");
  
    }
    else if(HCSR04_Distance > 99)
   {
      HCSR04_Distance = HCSR04_Distance/100;
      lcd.print("Distance: ");
      lcd.print(String(HCSR04_Distance));
      lcd.print("m");
      lcd.print("    ");
    }
    else
    {
      lcd.print("Distance: ");
      lcd.print(String(HCSR04_Distance));
      lcd.print("cm");
      lcd.print("    ");
    }  
}

void LCDPrintReceivables(float, int, bool, byte, byte)
{
   Wire.setClock(10000);
   lcd.setCursor(2,0);
   Convert_Meters_Or_AboveMax(HCSR04_Distance);
   //lcd.setCursor(0,0);
   //lcd.print("Distance: ");
   //lcd.print(String(HCSR04_Distance));
   //lcd.print("cm");
   lcd.setCursor(5,1);
   lcd.print("Temp: ");
   lcd.print(String(DHT11_Temperature));
   lcd.print((char)223); //Degree symbol
   lcd.print("C");
   lcd.setCursor(4,2);
   lcd.print("Humidity: ");
   lcd.print(String(DHT11_Humidity));
   lcd.print("%");
   lcd.setCursor(0,3);
   lcd.print("Battery: ");
   //lcd.setCursor(11,3);
   lcd.print("% template"); 
   
   delay(250);
}


void SerialPrintReceivables(float, int, bool, byte, byte)
{
  Serial.println("This is a Serial Monitor test for imported values");
  Serial.print("HCSR04_Distance: ");
  Serial.print(HCSR04_Distance);
  
  Serial.println("cm");
  Serial.print("Buzzer_Frequency: ");
  Serial.println(Buzzer_Frequency);
  Serial.print("isBeeping: ");
  Serial.println(isBeeping);
  Serial.print("DHT11_Temperature: ");
  Serial.print(DHT11_Temperature);
  Serial.println(" *C");
  Serial.print("DHT11_Humidity: ");
  Serial.print(DHT11_Humidity);
  Serial.println(" H");
  Serial.println("*******************************************");

  delay(250); 
}

void receiveEvent (int howMany)
 {
   I2C_readAnything (HCSR04_Distance);
   I2C_readAnything (Buzzer_Frequency);
   I2C_readAnything (isBeeping);
   I2C_readAnything (DHT11_Temperature);
   I2C_readAnything (DHT11_Humidity);
   haveData = true;
 }
