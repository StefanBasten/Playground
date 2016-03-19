#include <DallasTemperature.h>
#include <OneWire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "Mudbus.h"
#define ONE_WIRE_BUS 2 /* Digitalport Pin 2 definieren */
OneWire ourWire(ONE_WIRE_BUS); /* Ini oneWire instance */
Adafruit_BMP085 bmp;
DallasTemperature sensors(&ourWire);/* Dallas Temperature Library für Nutzung der oneWire Library vorbereiten */

Mudbus Mb;
//Function codes 1(read coils), 3(read registers), 5(write coil), 6(write register)
//signed int Mb.R[0 to 125] and bool Mb.C[0 to 128] MB_N_R MB_N_C
//Port 502 (defined in Mudbus.h) MB_PORT

void setup()
{
  uint8_t mac[]     = { 0x90, 0xA2, 0xDA, 0x00, 0x51, 0x06 };
  uint8_t ip[]      = { 192, 168, 178, 78 };
  uint8_t gateway[] = { 192, 168, 178, 1 };
  uint8_t subnet[]  = { 255, 255, 255, 0 };
  Ethernet.begin(mac, ip, gateway, subnet);
  //Avoid pins 4,10,11,12,13 when using ethernet shield

  delay(5000);
  Serial.begin(9600);
  sensors.begin();/* Inizialisieren der Dallas Temperature library */
  bmp.begin();
  pinMode(7, INPUT);
  pinMode(8, OUTPUT); 
}

void loop()
{
  Mb.Run();
  sensors.requestTemperatures(); // Temp abfragen
  //Analog inputs 0-1023
  Mb.R[0] = analogRead(A0); //pin A0 to Mb.R[0]
  Mb.R[1] = (bmp.readAltitude(97600)*10);
  Mb.R[2] = bmp.readAltitude();
  //Mb.R[3] = (bmp.readSealevelPressure()/9.6865);
  Mb.R[3] = ((bmp.readSealevelPressure()+4600)/10);
  Mb.R[4] = (bmp.readTemperature()*100);
  Mb.R[5] = (bmp.readPressure()/10);
  Mb.R[6] = (sensors.getTempCByIndex(0)*100);
  Mb.R[7] = (sensors.getTempCByIndex(1)*100); 
  Mb.R[9] = (sensors.getTempCByIndex(0)*100);
  Mb.R[10] = (sensors.getTempCByIndex(1)*100);
 
  //Digital outputs
  digitalWrite(8, Mb.C[8]); //pin 8 from Mb.C[8]
Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
Serial.print("Pressuresealevel = ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println(" Pa");
    
}


