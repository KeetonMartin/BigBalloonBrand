#include <SoftwareSerial.h>
#include "TinyGPS++.h"
#include "DHT.h"
#include <ADJDS311.h>
#include <Wire.h>

//for pressure
int pressurePin = A4;

//for sarah
int sensorLed_pin = 2; //LED on the ADJDS-311
ADJDS311 colorSensor(sensorLed_pin);
//if using an RGB LED (Needs PWM Pins)
int redPin = 3;
int greenPin = 5;
int bluePin = 6;

//for DHT sensor
#define DHTPIN A5     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);
//All this is for the humidity ^
//TMP36 Pin Variables
int sensorPin0 = 0; //the analog pin the TMP36's Vout (sense) pin is connected to
                        //the resolution is 10 mV / degree centigrade with a
                        //500 mV offset to allow for negative temperatures
int sensorPin1 = 0;

//Thomas
// Analog pin 1 will be called 'sensor'
int sensor = A0;

// for GPS
SoftwareSerial gpsSerial(10, 11); // RX, TX (TX not used)
const int sentenceSize = 80;
char sentence[sentenceSize];
// Create a TinyGPS++ object called "gps"
TinyGPSPlus gps;

//variables we're gonna be using throughout methods
int HOUR;
int MINUTE;
double TEMPDHT;
double TEMPTHERM;
double HUMIDITY;
double ALTITUDE;
double LAT;
double LON;
int NUMSATS;
double PRESSURE;


void setup()
{
  Serial.begin(9600);
  gpsSerial.begin(9600);
  dht.begin();

  colorSensor.init();
  colorSensor.ledOn(); //turn LED on
  
  //Calibrate white 
  //Need to hold white card in front (1-3mm) of it to calibrate from
  colorSensor.calibrate(); 

}

void loop()
{
//  static int i = 0;
//  if (gpsSerial.available())
//  {
//    char ch = gpsSerial.read();
//    if (ch != '\n' && i < sentenceSize)
//    {
//      sentence[i] = ch;
//      i++;
//    }
//    else
//    {
//     sentence[i] = '\0';
//     i = 0;
//     displayInfo();
//    }
//  }

  if (gpsSerial.available() > 0)
  {
      if (gps.encode(gpsSerial.read()))
    {
      displayInfo();
    }
  }
}

//void displayGPS()
//{
//  char field[20];
//  getField(field, 0);
//  if (strcmp(field, "$GPRMC") == 0)
//  {
//    Serial.print("Lat: ");
//    getField(field, 3);  // number
//    Serial.print(field);
//    getField(field, 4); // N/S
//    Serial.print(field);
//    
//    Serial.print(" Long: ");
//    getField(field, 5);  // number
//    Serial.print(field);
//    getField(field, 6);  // E/W
//    Serial.println(field);
//  }
//}

String coleEx(String ourHOUR, String ourMINUTE, String ourALTITUDE, String ourHUMIDITY)
{
  return "Time: " + ourHOUR + ":" + ourMINUTE + " Altitude: " + ourALTITUDE + " Humidity: " + ourHUMIDITY;
}

void displayInfo()
{
  //line 1
//  Serial.print("Satellites Connected: "); Serial.println(gps.satellites.value());
  NUMSATS = gps.satellites.value();
  
  //line 2
//  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {    
//    Serial.print(gps.location.lat(), 6);
    LAT = (gps.location.lat());
//    Serial.print(F(","));
//    Serial.print(gps.location.lng(), 6);
    LON = (gps.location.lng());
//    Serial.print(F(", "));
//    Serial.print("Altitude:"); Serial.print(gps.altitude.feet());
    ALTITUDE = gps.altitude.feet();
  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.println();
  
  //line 3
  //DHT time
  //This is all from Chip
    HUMIDITY = dht.readHumidity();
  // Read temperature as Celsius
    TEMPDHT = dht.readTemperature();

  if (isnan(HUMIDITY) || isnan(TEMPDHT)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

//  Serial.print("Humidity: "); 
//  Serial.print(HUMIDITY);
//  Serial.print(" %\t");
//  Serial.print("Temperature: "); 
//  Serial.print(TEMPDHT);
//  Serial.println(" *C ");

  //line 4
//  Serial.print("Time (Pacific): "); Serial.print((gps.time.hour())-7);
  HOUR = gps.time.hour()-7;
//  Serial.print(":"); Serial.println(gps.time.minute());
  MINUTE = gps.time.minute();

  //Serial.print(HOUR); Serial.print(","); Serial.print(MINUTE); Serial.print(","); Serial.print(TEMPDHT); Serial.print(","); Serial.print(TEMPTHERM); 
  //Serial.print(","); Serial.print(HUMIDITY); Serial.print(","); Serial.print(ALTITUDE); Serial.print(","); Serial.print(LAT); Serial.print(",");
  //Serial.print(LON); Serial.print(","); Serial.print(NUMSATS);

  Serial.print("Number of Satellites we can see: ");
  Serial.println(NUMSATS);

  Serial.print("Thomas Data: ");
  Serial.println(thomasEx(ALTITUDE));
  Serial.println(sarahEx());

  Serial.print("Cole Experiment");
  Serial.println(coleEx((String)HOUR, (String)MINUTE, (String)ALTITUDE, (String)HUMIDITY));

  //line 5
  PRESSURE = analogRead(pressurePin);
  Serial.print("Pressure: ");
  Serial.println(PRESSURE);

  //line 6
  //this is all from therma resistor:
   //getting the voltage reading from the temperature sensor
   int reading0 = analogRead(sensorPin0);
   // converting that reading to voltage, for 3.3v arduino use 3.3
   float voltage = reading0 * 5.0;
   voltage /= 1024.0; 
   // now print out the temperature
   //Calcs for interior temp
   double d1 = 5.0 - (double)voltage;
   double d2 = 5.0 - d1;
   double current = d2 / 49000.0;
   double varResistance = d1 / current;
   double TEMPTHERM = equation(varResistance);

   Serial.print("ThermaTemp: ");
   Serial.println(TEMPTHERM);
  
  delay(10000);
}

void getField(char* buffer, int index)
{
  int sentencePos = 0;
  int fieldPos = 0;
  int commaCount = 0;
  while (sentencePos < sentenceSize)
  {
    if (sentence[sentencePos] == ',')
    {
      commaCount ++;
      sentencePos ++;
    }
    if (commaCount == index)
    {
      buffer[fieldPos] = sentence[sentencePos];
      fieldPos ++;
    }
    sentencePos ++;
  }
  buffer[fieldPos] = '\0';
} 

String thomasEx(double alt)
{
    // Read the input on analog pin 1 (named 'sensor')
    double ozone = analogRead(sensor);
    return "ozone: " + (String)ozone + "altitude: " + (String)alt;
}

void lightLED(RGBC color)
{
  analogWrite(redPin, map(color.red, 0, 255, 255, 255));
  analogWrite(greenPin, map(color.green, 255, 0, 255, 255));
  analogWrite(bluePin, map(color.blue, 255, 255, 0, 255));
}

String sarahEx()
{
  RGBC color = colorSensor.read(); //read the color
  String colorOutput = String(color.red) + " | " + String(color.green) + " | " + String(color.blue) + " | " + String(color.clear);
  lightLED(color); //send color to the LED
  return colorOutput;
}


double equation(double varResistance)
{
  return pow (2.71, ((-1*varResistance + 33009)/7004));
}


