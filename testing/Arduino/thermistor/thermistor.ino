//Thermometer with thermistor

/*thermistor parameters:
 * RT0: 10 000 Ω
 * B: 3977 K +- 0.75%
 * T0:  25 C
 * +- 5%
 */
#include <ArduinoBLE.h>

BLEService ledService("180A");
BLEByteCharacteristic switchCharacteristic("2A57", BLERead | BLEWrite);

//These values are in the datasheet
#define RT0 10000   // Ω
#define B 3950      // K
//--------------------------------------


#define VCC 5    //Supply voltage
#define R 10000  //R=10KΩ

//Variables
float RT, VR, ln, TX, T0, VRT;
const int ledPin = LED_BUILTIN;
const int samples = 1000;

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  T0 = 25 + 273.15;                 //Temperature T0 from datasheet, conversion from Celsius to kelvin
}

void loop() {
  
  float sum = 0;
  for (int i=0; i < samples; i++) {
    VRT = analogRead(A7);              //Acquisition analog value of VRT
    VRT = (5.00 / 1023.00) * VRT;      //Conversion to voltage
    VR = VCC - VRT;
    RT = VRT / (VR / R);               //Resistance of RT
  
    ln = log(RT / RT0);
    TX = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor
  
    TX = TX - 273.15;                 //Conversion to Celsius
    sum += TX;
  }

  float average = sum / samples;

  if(average >= 30){
    digitalWrite(ledPin, HIGH);
    }
  else {
    digitalWrite(ledPin, LOW);
  }
  
  Serial.print("Temperature:");
  Serial.print("\t");
  Serial.print(average);
  Serial.print("C\t\t");
  Serial.print(average + 273.15);        //Conversion to Kelvin
  Serial.print("K\t\t");
  Serial.print((average * 1.8) + 32);    //Conversion to Fahrenheit
  Serial.println("F");
}
