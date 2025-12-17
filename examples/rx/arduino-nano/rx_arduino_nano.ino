#include <SPI.h>
#include <RF24.h>
#include "../../include/telemetry_packet.h"

// Arduino Nano pins example
// CE -> D9, CSN -> D10 (adjust if wired differently)
static const uint8_t NRF_CE = 9;
static const uint8_t NRF_CSN = 10;
static const byte PIPE_ADDR[6] = "24613";

RF24 radio(NRF_CE, NRF_CSN);
TelemetryPacket pkt;

void setup() {
  Serial.begin(115200);
  if (!radio.begin()) {
    Serial.println("NRF24 init failed");
    while (true) { }
  }
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, PIPE_ADDR);
  radio.startListening();
  Serial.println("RX(Arduino Nano) ready");
}

void loop() {
  if (radio.available()) {
    radio.read(&pkt, sizeof(pkt));
    Serial.print("RX: ");
    Serial.print(pkt.lat, 7); Serial.print(", ");
    Serial.print(pkt.lon, 7); Serial.print(", ");
    Serial.print(pkt.alt, 2); Serial.print(", ");
    Serial.println(pkt.yaw, 2);
  }
}
