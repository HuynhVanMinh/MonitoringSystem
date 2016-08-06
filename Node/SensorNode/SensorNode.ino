#include "Arduino.h"

#include <TimerOne.h>

/**********Sensor Nhiet Do - Do Am******************/
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float DoAm;
float NhietDo;

/**********Sensor Do Am Dat******************/
#define DAD_PIN A0
unsigned int DAD;

/**********Sensor anh sang******************/
#include <Wire.h>
#include <BH1750.h>

BH1750 lightSensor;
uint16_t AnhSang;

/**************** Xbee S2 ************/
#include <SoftwareSerial.h>
SoftwareSerial xbeeSerial(6, 5); // RX, TX
#define xbeeReset 4
#define xbeeWake 7
#include <XBee.h>
XBee xbee = XBee();
#define UnicastAddress 0x0
XBeeAddress64 addressControlCenter = XBeeAddress64(UnicastAddress);
uint8_t payload[200];
ZBTxRequest ZBTx = ZBTxRequest(addressControlCenter, payload, 200);

void xbeeSend() {
  String Data = "\"ND\":" + String((int) NhietDo) + ",\"DA\":"
                + String((int) DoAm) + ",\"DAD\":" + String((int) DAD) + ",\"AS\":"
                + String((int) AnhSang);
  strcpy((char*) payload, Data.c_str());
  ZBTx.setPayloadLength((uint8_t) Data.length());
  digitalWrite(xbeeWake, LOW);
  delay(500);
  xbee.send(ZBTx);
  delay(500);
  digitalWrite(xbeeWake, HIGH);
}

//The setup function is called once at startup of the sketch
void setup() {
  // Add your initialization code here
  Serial.begin(9600);

  /* Initialization xbee */
  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
  pinMode(xbeeReset, OUTPUT);
  pinMode(xbeeWake, OUTPUT);
  //Reset Xbee
  digitalWrite(xbeeReset, LOW);
  delay(500);
  digitalWrite(xbeeReset, HIGH);

  /* Initialization Light Sensor */
  lightSensor.begin();
  AnhSang = lightSensor.readLightLevel();

  /* Initialization Sensor Nhiet Do - Do Am */
  dht.begin();
  DoAm = dht.readHumidity();
  NhietDo = dht.readHumidity();

  /* Initialization Sensor Do Am Dat*/
  DAD = (1024 - analogRead(DAD_PIN)) / 10.24;

  /* Initialization Timer*/
  //Timer1.initialize(2000000); // set a timer of length 5second
  //Timer1.attachInterrupt(timerIsr); // attach the service routine here


  /* Initialization finish*/
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

}

// The loop function is called in an endless loop
void loop() {
  //Add your repeated code here
  AnhSang = lightSensor.readLightLevel();
  DoAm = dht.readHumidity();
  NhietDo = dht.readTemperature();
  DAD = (1024 - analogRead(DAD_PIN)) / 10.24;
  Serial.println(DAD);
  xbeeSend();
  delay(5000);
}

/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr() {
  xbeeSend();
}
