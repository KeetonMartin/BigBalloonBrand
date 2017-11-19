//for temperature (thermistor)
  int sensorPin0 = 0;
//for pressure
  int pressurePin = A4;
//for ozone
  // Digital pin 8 will be called 'tomDig'
  int tomDig = 8;
  // Analog pin 1 will be called 'ozonePin'
  int ozonePin = A1;
//for GPS
  #include <SoftwareSerial.h>
  #include "TinyGPS++.h"
  SoftwareSerial gpsSerial(10, 11); // RX, TX (TX not used)
  const int sentenceSize = 80;
  char sentence[sentenceSize];
  // Create a TinyGPS++ object called "gps"
  TinyGPSPlus gps;
//for radio
  String START_FILTER = "nXTs4";
  String END_FILTER = "PuoWQ";
  #define DELAY 500
  int prev = millis();
  boolean gpsWorks = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
//  Serial1.begin(9600);

  //for Ozone
  // Initialize the digital pin 8 as output
  pinMode(tomDig, OUTPUT);
  //set high voltage for supply to ozonePin
  digitalWrite(tomDig, HIGH);

  //for GPS
  gpsSerial.begin(9600);

  Serial.println("Initializing the system... Local Time: ");
  //  Serial.print("Time (Pacific): "); Serial.print((gps.time.hour())-7);
  int HOUR = gps.time.hour()-8;
//  Serial.print(":"); Serial.println(gps.time.minute());
  int MINUTE = gps.time.minute();
  Serial.println(String(HOUR) + ":" + String(MINUTE));
}

void loop() {
  //EVERY 30 SECONDS PRINT OUT A BUNCH OF DATA ON NON GPS STUFF
  if(((millis())/1000)%30 == 0)
  {
    //Print Temperature
    int reading0 = analogRead(sensorPin0);  
    Serial.println(voltToTemperature(reading0));

    //print pressure value (around 500)
    int pressure = analogRead(pressurePin);
    Serial.print("Pressure: ");
    Serial.println(pressure);

    //print ozone reading (around 900 in the lab)
    int ozone = analogRead(ozonePin);
    Serial.println(String(ozone) + "<-- Ozone Reading");
    prev = millis();
    gpsWorks = false;
  }
  //EVERY 30 SECONDS (OFFSET BY 15) PRINT GPS DATA IF YOU CAN!
  else
  {
    //Print GPS info
    if (gpsSerial.available() > 0)
    {
       if (gps.encode(gpsSerial.read()))
       {
          displayGPS();
          gpsWorks = true;
       }
    }
  }
  
  
  //old
//  if(millis() > prev + DELAY || gpsWorks) {
//    //Print Temperature
//    int reading0 = analogRead(sensorPin0);  
//    Serial.println(voltToTemperature(reading0));
//
//    //print pressure value (around 500)
//    int pressure = analogRead(pressurePin);
//    Serial.print("Pressure: ");
//    Serial.println(pressure);
//
//    //print ozone reading (around 900 in the lab)
//    int ozone = analogRead(ozonePin);
//    Serial.println(String(ozone) + "<-- Ozone Reading");
//    prev = millis();
//    gpsWorks = false;
//  }
//
//  //Print GPS info
//  if (gpsSerial.available() > 0)
//  {
//      if (gps.encode(gpsSerial.read()))
//    {
//      displayGPS();
//      gpsWorks = true;
//    }
//  }
}

void displayGPS()
{
  if((((millis())/1000)+15)%30 == 0)
  {
    //line 1
//  Serial.print("Satellites Connected: "); Serial.println(gps.satellites.value());
  int NUMSATS = gps.satellites.value();
  
  //line 2
//  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {    
//    Serial.print(gps.location.lat(), 6);
    double LAT = (gps.location.lat());
//    Serial.print(F(","));
//    Serial.print(gps.location.lng(), 6);
    double LON = (gps.location.lng());
//    Serial.print(F(", "));
//    Serial.print("Altitude:"); Serial.print(gps.altitude.feet());
    int ALTITUDE = gps.altitude.feet();
    Serial.println("Location (Lat, Lon, Alt): " + String(LAT) + "," + String(LON) + "," + String(ALTITUDE));
  }
  else
  {
    //Serial.print(F("INVALID"));
  }

  //line 4
//  Serial.print("Time (Pacific): "); Serial.print((gps.time.hour())-7);
  int HOUR = gps.time.hour()-8;
//  Serial.print(":"); Serial.println(gps.time.minute());
  int MINUTE = gps.time.minute();
  //Serial.println("Time: " + String(HOUR) + ":" + String(MINUTE));

  //Serial.print(HOUR); Serial.print(","); Serial.print(MINUTE); Serial.print(","); Serial.print(TEMPDHT); Serial.print(","); Serial.print(TEMPTHERM); 
  //Serial.print(","); Serial.print(HUMIDITY); Serial.print(","); Serial.print(ALTITUDE); Serial.print(","); Serial.print(LAT); Serial.print(",");
  //Serial.print(LON); Serial.print(","); Serial.print(NUMSATS);

  //Serial.print("Number of Satellites we can see: ");
  //Serial.println(NUMSATS);
  
//  delay(10000);
  }
}

String voltToTemperature(int volt) 
{
 // converting that reading to voltage, for 3.3v arduino use 3.3
 double voltage = volt * 5.0;
 voltage /= 1024.0; 
 // now print out the temperature
 //Calcs for interior temp
 double d1 = 5.0 - (double)voltage;
 double d2 = 5.0 - d1;
 double current = d2 / 49000.0;
 double varResistance = d1 / current;
 
 double temp = equation(varResistance);
// Serial.print(temp); Serial.println("Degrees C Official Yay");
 // now convert to Fahrenheit
 float temperatureF = (temp * 9.0 / 5.0) + 32.0;
// Serial.print(temperatureF); Serial.println(" degrees F");
 return String(temp) + " degrees C, " + String(temperatureF) + "degrees F";
}

double equation(double varResistance)
{
  return pow (2.71, ((-1*varResistance + 33009)/7004));
}

void send(String message) 
{
  //Turn on transmit mode
  digitalWrite(3, HIGH);
  delay(10);
  String s = START_FILTER + message + END_FILTER;

  //Send message multiple times so it definetly receives
  for (int i = 0; i < 10; i += 1) {
     Serial1.print(s);
  }
  //Turn off transmit mode
  delay(10);
  digitalWrite(3, LOW);
  delay(10);
}
