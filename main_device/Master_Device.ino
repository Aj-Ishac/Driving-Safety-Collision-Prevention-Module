#include "Arduino.h"
//#include "dht.h"
//#include <SoftwareSerial.h>

//Define pins when hardware arrives!
const int Trigger_Pin = 3;
const int Echo_PIN = 2;
***************************************************************
//const int Buzzer_Pin = 4;
/***************************************************************
  const int DHT_Pin DHT_PinNum
  //***************************************************************
  const int LED_RedPin  LED_RedPinNum
  const int LED_GrnPin  LED_GreenPinNum
  //***************************************************************
  const int Button_Pin  ButtonPin_Num
  //***************************************************************
  SoftwareSerial BTserial(rxPin, txPin); // rxPin, txPin*/


void setup()
{

  Serial.begin(9600);                 //Default communication channel
  Serial.println("Serial channel is ready at 9600");    //temp for testing purposes
  delay(2000);
  //BTserial.begin(38400);              //Bluetooth communication channel
  //Serial.println("BTserial channel is ready at 38400"); //temp for testing purposes
  //delay(2000);

  //Ultrasonic Sensor
  pinMode(Trigger_Pin, OUTPUT);
  pinMode(Echo_PIN, INPUT);

  //Buzzer
  //pinMode(Buzzer_Pin, OUTPUT);
  /*
    //LED RED&GRN
    pinMode(LED_RedPin,OUTPUT);
    pinMode(LED_GrnPin,OUTPUT);

    //Button
    pinMode(Button_Pin, INPUT);*/


}

void loop()
{

  //digitalWrite(LED_RedPin,HIGH);           //Red LED on when powered.

  Ultrasonic_CollisionDetect(Trigger_Pin, Echo_PIN);
  //TempHumidity_Detect(DHT_Pin);
  //GreenLED_Blink(LED_GrnPin);            //To be used for debugging at a later date.

}

void Ultrasonic_CollisionDetect(int, int)
{

  digitalWrite(Trigger_Pin, LOW);
  delayMicroseconds(2);
  digitalWrite(Trigger_Pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger_Pin, LOW);

  //Limitation is 20mm-4000mm
  int Min_Detection = 400;               //***Min Detection Controller***  current: 0.4m
  int Max_Detection = 3000;              //***Max Detection Controller***  current: 3.0m

  long Duration;                          //Duration of signal to leave TriggerPin and return to EchoPin
  float Distance;                         //Calculated distance in centimeters
  //int BuzzerFreq;
  //bool isBeeping;


  Duration = pulseIn(Echo_PIN, HIGH);     //Duration signal takes to traverse from TriggerPin to EchoPin
  Distance = (Duration * 0.034) / 2;      //Distance calculation in cm
  BuzzerFreq = Distance * 10;             //***Buzzer Frequency Controller***

  /*if (Distance > Max_Detection){           //Turn off buzzer when higher than min distance
    digitalWrite(Buzzer_Pin,LOW);         //analogWrite(Buzzer_Pin,0-255)
    //isBeeping = false;
    else
  {
    if (Distance < Min_Detection)     //Turn on buzzer to max frequency when lower than min distance
      digitalWrite(Buzzer_Pin,HIGH);
      //isBeeping = true;
      else
    { //If distance is between min and max distance, frequency of buzzing dependent on BuzzerFreq/Distance
      digitalWrite(Buzzer_Pin,HIGH);
      delay(BuzzerFreq);
      digitalWrite(Buzzer_Pin,LOW);
      //isBeeping = true;

      }
    }
  }

  Serial.print("Distance: ");
  Serial.println(Distance);
  
  /*


  //return isBeeping;

}


//yellow ground
//orange 5v
//red 2
//brown 3








/*int DataToTransmit()//Master BT Communication
  {

  if(Serial.available() > 0)
  {
    BT_Data = Serial.read();
    state = 0;
  }
  if(info == '1')
  {



  }

  else if(info == '0'){






  if(BTserial.available()              //Read if available data exists in serial port
    Serial.write(BTserial.read());

  if(Serial.available())
  BTSerial.write(Serial.read());

  }

  void GreenLED_Blink(int)
  {

  int Led_State = LOW;
  unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  const long interval = 1000;

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);



  void TempHumidity_Detect(int)
  {

  int DHT11_Check;

  DHT11_Check = DHT.read11(DHT11_PIN);
  Serial.print("Temperature: ");
  Serial.println(DHT.temperature);
  delay(1000)
  Serial.print("Humidity: ");
  Serial.println(DHT.humidity);
  delay(1000);

  }

*/


//***main device***
//ulrasonic work HC-SR04        DONE
//buzzer Piezzo Buzzer          DONE
//humidity and temp DHT11       DONE
//led                           DONE
//button                        DONE
//bluetooth HC-05               DONE
//battery                       NEXT

//***alt device***
//bluetooth HC-05
//buzzer Piezzo Buzzer
//led
//button
//battery



//analogWrite(Buzzer_Pin,0-255) vs digitalWrite(Buzzer_Pin,LOW-HIGH)
//https://arduino.stackexchange.com/questions/35873/whats-the-difference-between-analogwrite-and-digitalwrite
//HC-SR04 AND PIEZZO BUZZER
//https://github.com/d03n3rfr1tz3/HC-SR04
//DHT11
//https://desire.giesecke.tk/index.php/2018/01/30/esp32-dht11/
//https://github.com/adafruit/DHT-sensor-library
//https://www.youtube.com/watch?v=OogldLc9uYc
//https://toptechboy.com/arduino-tutorial-50-how-to-connect-and-use-the-dht11-temperature-and-humidity-sensor/
//Sometimes this resistor is already integrated in the module, sometimes its necessary to add it.
//Library Setup
//https://www.arduino.cc/en/Hacking/LibraryTutorial
//button func
//You can use the button as a switcher for the Arduino power. Just wire up the button between the battery + out and the Arduino's Vin port.
//Connecting 2 Arduinos by Bluetooth using a HC-05 and a HC-06: Pair, Bind, and Link
//http://www.martyncurrey.com/connecting-2-arduinos-by-bluetooth-using-a-hc-05-and-a-hc-06-pair-bind-and-link/
//https://howtomechatronics.com/tutorials/arduino/how-to-configure-pair-two-hc-05-bluetooth-module-master-slave-commands/
//https://www.youtube.com/watch?v=afGxMfy7_0A
//master to send data to slave and once confirmation is received, green led lights up as confirmation visual
//https://www.youtube.com/watch?v=3WR2-HT_MiQ
