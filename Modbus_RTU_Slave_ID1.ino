
#include <ModbusRtu.h>
#include "DHT.h"
#define DHTPIN 2     
#define DHTTYPE DHT21 
DHT dht(DHTPIN, DHTTYPE);

#define ID   1 //  Modbus Slave ID
#define TXEN  4 // der Pin des Arduinos der mit den gebrückten Pins RE-DE der RS485 Transceiverplatine verbunden sein muss

/**
 *  Modbus Object Beschreibung
 *  u8id : node ID = 0 als master, = 1..247 als slave
 *  u8serno : serial port (benutze 0 für serielle Kommunikation)
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               oder einen Pin (TXEN) Nummer > 1 for RS-485
 */
Modbus slave(ID, 0, TXEN); // dieser ist Slave mit ID 1 und RS-485 mit Sendekontrolle Pin 4
boolean led;
int8_t state = 0;
unsigned long tempus;

// Datenarray für die Modbuskommunikation
uint16_t au16data[9];


void setup() {
  io_setup(); // I/O Settings
  delay(1000);
  slave.begin( 19200 ); // Start der KommuniKation mit 19200 Baudrate
  tempus = millis() + 100;
  digitalWrite(13, HIGH );
}

 //  Schleife
 
void loop() {
  // poll Nachrichten
  // blinken der LED bei jeder gültigen Nachricht
  state = slave.poll( au16data, 9 );
  if (state > 4) {
    tempus = millis() + 50;
    digitalWrite(13, HIGH);
  }
  if (millis() > tempus) digitalWrite(13, LOW );

  // Rangierung der Arduinopins zum Modbusarray
  io_poll();
} 

/**
 * pin maping:
 * 2 - digital input
 * 3 - digital input
 * 4 - digital input
 * 5 - digital input
 * 6 - digital output
 * 7 - digital output
 * 8 - digital output
 * 9 - digital output
 * 10 - analog output
 * 11 - analog output
 * 14 - analog input
 * 15 - analog input
 *
 * pin 13 is reserved to show a successful query
 */
void io_setup() {
  // define i/o
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(6, LOW );
  digitalWrite(7, LOW );
  digitalWrite(8, LOW );
  digitalWrite(9, LOW );
  digitalWrite(13, HIGH ); // this is for the UNO led pin
  analogWrite(10, 0 );
  analogWrite(11, 0 );
}

/**
 *  Link between the Arduino pins and the Modbus array
 */
void io_poll() {


   float h = dht.readHumidity();
   float t = dht.readTemperature();
   int b = h*10;
   int c = t*10;
  // get digital inputs -> au16data[0]
  bitWrite( au16data[0], 0, digitalRead( 2 ));
  bitWrite( au16data[0], 1, digitalRead( 3 ));
  bitWrite( au16data[0], 2, digitalRead( 4 ));
  bitWrite( au16data[0], 3, digitalRead( 5 ));

  // set digital outputs -> au16data[1]
  digitalWrite( 6, bitRead( au16data[1], 0 ));
  digitalWrite( 7, bitRead( au16data[1], 1 ));
  digitalWrite( 8, bitRead( au16data[1], 2 ));
  digitalWrite( 9, bitRead( au16data[1], 3 ));

  // set analog outputs
  analogWrite( 10, au16data[2] );
  analogWrite( 11, au16data[3] );

  // read analog inputs
  au16data[4] = (b*10);
  au16data[5] = (c*10);

  // diagnose communication
  au16data[6] = slave.getInCnt();
  au16data[7] = slave.getOutCnt();
  au16data[8] = slave.getErrCnt();
}