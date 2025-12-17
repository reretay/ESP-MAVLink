#include <Arduino.h>
#include "../../../include/mavlink_include.h"

// Minimal TELEM link test: send MAVLink heartbeat only (no RF)
// Default pins for ESP32-C3 SuperMini
#ifndef MAV_RX_PIN
#define MAV_RX_PIN 20
#endif
#ifndef MAV_TX_PIN
#define MAV_TX_PIN 21
#endif
#ifndef MAV_BAUD
#define MAV_BAUD 230400
#endif

unsigned long lastTick = 0;
const unsigned long intervalMs = 1000;

static void send_heartbeat() {
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];
  mavlink_msg_heartbeat_pack(255, 0, &msg, MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID,
                             MAV_MODE_MANUAL_ARMED, 0, MAV_STATE_ACTIVE);
  const uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
  Serial1.write(buf, len);
}

void setup() {
  Serial.begin(115200);
  Serial.println("TELEM Heartbeat-only test");
  Serial1.begin(MAV_BAUD, SERIAL_8N1, MAV_RX_PIN, MAV_TX_PIN);
}

void loop() {
  const unsigned long now = millis();
  if (now - lastTick >= intervalMs) {
    lastTick = now;
    send_heartbeat();
    Serial.println("HB TX");
  }
}
