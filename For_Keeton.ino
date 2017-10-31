#include "DHT.h"
 
#define DHTPIN A5     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
 
DHT dht(DHTPIN, DHTTYPE);
//All this is for the humidity ^

//TMP36 Pin Variables
int sensorPin0 = 0; //the analog pin the TMP36's Vout (sense) pin is connected to
                        //the resolution is 10 mV / degree centigrade with a
                        //500 mV offset to allow for negative temperatures
int sensorPin1 = 0;
/*
 * setup() - this function runs once when you turn your Arduino on
 * We initialize the serial connection with the computer
 */
void setup()
{
  Serial.begin(9600);  //Start the serial connection with the computer
                       //to view the result open the serial monitor 
  dht.begin();

}
 
void loop()                     // run over and over again
{
  float voltageH = analogRead(sensorPin1);
  Serial.print(voltageH);Serial.println("voltage for Cole");
  
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

  //this is all from therma resistor:
 //getting the voltage reading from the temperature sensor
 int reading0 = analogRead(sensorPin0);  
 Serial.println(reading0);

 int reading1 = analogRead(sensorPin1);
 Serial.println(reading1);
 Serial.println("This is after reading 1: ");
 
 // converting that reading to voltage, for 3.3v arduino use 3.3
 float voltage = reading0 * 5.0;
 voltage /= 1024.0; 
 
 // print out the voltage
 Serial.print(voltage); Serial.println(" volts");

 // now print out the temperature
 //Calcs for interior temp
 double d1 = 5.0 - (double)voltage;
 double d2 = 5.0 - d1;
 double current = d2 / 49000.0;
 double varResistance = d1 / current;

 double temp = equation(varResistance);
 Serial.print(temp); Serial.println("Degrees C Official Yay");
 
 // now convert to Fahrenheit
 float temperatureF = (temp * 9.0 / 5.0) + 32.0;
 Serial.print(temperatureF); Serial.println(" degrees F");

 delay(5000);                                     //waiting a second

 //sending data via radio
 send(
}

void send()
{
  
}

double equation(double varResistance)
{
  return pow (2.71, ((-1*varResistance + 33009)/7004));
}

void send(String message)
{
  
}

