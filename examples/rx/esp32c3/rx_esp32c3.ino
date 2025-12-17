#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "telemetry_packet.h"

static const uint8_t NRF_CE = 10;
static const uint8_t NRF_CSN = 9;
static const byte PIPE_ADDR[6] = "24613";

RF24 radio(NRF_CE, NRF_CSN);
TelemetryPacket pkt;

void setup() {
  Serial.begin(115200);
  if (!radio.begin()) { Serial.println("NRF24 init failed"); while (true) delay(100);} 
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, PIPE_ADDR);
  radio.startListening();
  Serial.println("RX(ESP32) ready");
}

void loop() {
  if (radio.available()) {
    radio.read(&pkt, sizeof(pkt));
    Serial.printf("RX: lat=%.7f lon=%.7f alt=%.2f yaw=%.2f\n", pkt.lat, pkt.lon, pkt.alt, pkt.yaw);
  }
}
