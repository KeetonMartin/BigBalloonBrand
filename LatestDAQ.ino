#include <SoftwareSerial.h>
#include "TinyGPS++.h"
#include "DHT.h"

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

// for GPS
SoftwareSerial gpsSerial(10, 11); // RX, TX (TX not used)
const int sentenceSize = 80;
char sentence[sentenceSize];
// Create a TinyGPS++ object called "gps"
TinyGPSPlus gps;

void setup()
{
  Serial.begin(9600);
  gpsSerial.begin(9600);
  dht.begin();
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

void displayInfo()
{
  //line 1
  Serial.print("Satellites Connected: "); Serial.println(gps.satellites.value());
  //line 2
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {    
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    Serial.print(F(", "));
    Serial.print("Altitude:"); Serial.print(gps.altitude.feet());
  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.println();

  //line 3
  //DHT time
  //This is all from Chip
    float h = dht.readHumidity();
  // Read temperature as Celsius
    float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.println(" *C ");

  //line 4
  Serial.print("Time (Pacific): "); Serial.print((gps.time.hour())-7);
  Serial.print(":"); Serial.println(gps.time.minute());
  
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

