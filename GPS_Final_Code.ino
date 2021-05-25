//EECE 396 Topics: Final Project 
//Bryce Keever
//This code is part of a final project to be presented with the submitted report.
//  It is a GPS tracker that uses an OLED module & a GPS module.
//  Utilizing TinyGPS++ to parse NEMA data from the GPS module to display lat, long
//  & number of satellites connected for easy location tracking. 

#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <SH1106.h>
// A shootoff of the Adafruit GPX graphics libaray that is more condensed
// and works better with ESP32 and ESP8266 dev modules

// Variable Section //
// Floating both variables to be connected to the GPS module
float latitude , longitude, satellites;
// These string variables print the lat and long values
String  lat_str , lng_str, sat_str;


// Instantiation Section //
SH1106 display(0x3C, 21, 22); //OLED Screen
TinyGPSPlus gps; //GPS data parsing library
HardwareSerial SerialGPS(1); // In/Out for GPS to OS

// Define Section //
#define EEPROM_SIZE 128
// Data structures
struct GpsDataType {
    double originlat = 0; //These are used as zeros for the EEPROM and set as doubles
    double originlng = 0; //to read the values saved in the void loop
    double originsat = 0; 
};
//Array declaration for the double float variables to hold a value
GpsDataType gpsstate = {};

// Flash Memory Section //
//Writing and reading to EEPROM flash memory on the ESP32 Board from Arduino
template <class T> int EEPROM_readAnything(int ee, T& value)
 {
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
 }

template <class T> int EEPROM_writeAnything(int ee, const T& value)
 {
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

// One time instance
void setup()
{
  //Serial connection
  Serial.begin(115200);
  //Serial command for data collection PLX-DAQ
  Serial.println("CLEARDATA"); 
  //LABEL command creates label 
  Serial.println("LABEL,Computer Time,Latitude,Longitude");                           
  //Connects the ESP32 & GPS module to the serial connection 
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);

  //This block of code reads the values saved in the addresses of the flash memory.
  //The reason there is origin"whatever" is to allocate a space in the memory for
  //the program to read in the future. Because as seen above they are each doubles
  //to ensure the numbers saved at the addresses are fully read. 
  
  //EEPROM for permanent code flash
  while (!EEPROM.begin(EEPROM_SIZE)) {
        true;}
  long readValue; 
  EEPROM_readAnything(0, readValue);
  gpsstate.originlat = (double)readValue;
  EEPROM_readAnything(4, readValue);
  gpsstate.originlng = (double)readValue;
  EEPROM_readAnything(6, readValue);
  gpsstate.originsat = (double)readValue;

  //Initializes the OLED Screen
  display.init();
  display.flipScreenVertically();

}
// Forever Loop - repeates until stopped //
void loop()
{
  //If the GPS is available this entire section will run
  while (SerialGPS.available() > 0) {
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        //This block takes the instance of each section of NEMA data
        //parses it and the saves it in the corresponding floating variable 
        //and that is then converted to a string to be able to print on the OLED
        //& serial monitor
        latitude = gps.location.lat();
        lat_str = String(latitude , 6); //Six decimals
        longitude = gps.location.lng();
        lng_str = String(longitude , 6);
        satellites = gps.satellites.value();
        sat_str = String(satellites , 1); //One decimal

        

        long writeValue;
        writeValue = latitude;
        EEPROM_writeAnything(0, writeValue); //Savees "latitude" data in address 0
        writeValue = longitude;
        EEPROM_writeAnything(4, writeValue); //Savees "longitude" data in address 4
        writeValue = satellites;
        EEPROM_writeAnything(6, writeValue); //Savees "satellites" data in address 6

        //Serial monitor printouts for data collection
        //This command allows the PLX-DAQ to collect
        Serial.print("DATA,TIME,"); 
        Serial.print(lat_str); //the data contained within each string
        Serial.print(",");   //Seperated by a comma to define columns
        Serial.print(lng_str);
        delay(1000); //Updates every 1 second

        // OLED display Section //
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT); //Left to right text
        display.setFont(ArialMT_Plain_16); //Font
        display.drawString(0, 0, "Lat:"); //
        display.drawString(45, 0, lat_str);
        display.drawString(0, 20, "Long:");
        display.drawString(45, 20, lng_str);
        display.drawString(0, 40, "Sat:");
        display.drawString(45, 40, sat_str);
        display.display(); //refreshes screen when new NEMA data is updated
      }
     delay(1000);
     Serial.println();  
    }
  }  

}
