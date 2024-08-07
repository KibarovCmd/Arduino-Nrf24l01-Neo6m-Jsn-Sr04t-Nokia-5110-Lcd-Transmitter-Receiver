#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#define TRIG 7
#define ECHO 6
TinyGPSPlus gps;
SoftwareSerial ss(4, 3);
RF24 radio(9, 10);
const byte address[5] = { 0x12, 0x23, 0x45, 0x67, 0x89 };
float list[3];
void setup() {
  Serial.begin(9600);
  ss.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setChannel(0x76);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();
  radio.enableDynamicPayloads();  //If there is data loss during transmission, try removing this code.
  radio.powerUp();
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT_PULLUP);
}
void loop() {
  list[0] = gps.location.lat();
  list[1] = gps.location.lng();
  list[2] = distanceF();
  radio.write(&list, sizeof(list));
  Serial.println(list[0], 6);
  Serial.println(list[1], 6);
  Serial.println(list[2]);
  smartDelay(1000);
}
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available()) {
      gps.encode(ss.read());
    }
    if (millis() - start + 0.026022 < ms) {
      list[2] = distanceF();
    } else {
      list[2] = 0.0;
    }
    Serial.println(list[0], 6);
    Serial.println(list[1], 6);
    Serial.println(list[2]);
    radio.write(&list, sizeof(list));
  } while (millis() - start < ms);
}
int distanceF() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG, LOW);
  int distance = pulseIn(ECHO, HIGH, 26000);
  distance = distance / 58;
  return distance;
}
