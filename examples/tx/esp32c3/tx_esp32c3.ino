#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "../../include/mavlink_include.h"
#include "../../include/telemetry_packet.h"

// ESP32 classic example UART1 pins
static const int MAV_RX_PIN = 16;
static const int MAV_TX_PIN = 17;
static const uint32_t MAV_BAUD = 230400;

// NRF24L01
static const uint8_t NRF_CE = 10;
static const uint8_t NRF_CSN = 9;
static const byte PIPE_ADDR[6] = "24613";

RF24 radio(NRF_CE, NRF_CSN);
TelemetryPacket pkt;

unsigned long lastTick = 0;
const unsigned long intervalMs = 1000;

void send_heartbeat() {
  mavlink_message_t msg; uint8_t buf[MAVLINK_MAX_PACKET_LEN];
  mavlink_msg_heartbeat_pack(255, 0, &msg, MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID,
                             MAV_MODE_MANUAL_ARMED, 0, MAV_STATE_ACTIVE);
  const uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
  Serial1.write(buf, len);
}

void request_streams() {
  mavlink_message_t msg; uint8_t buf[MAVLINK_MAX_PACKET_LEN];
  const uint8_t streams[] = {MAV_DATA_STREAM_EXTENDED_STATUS, MAV_DATA_STREAM_EXTRA1};
  const uint16_t rates[]  = {2, 5};
  for (uint8_t i = 0; i < sizeof(streams); i++) {
    mavlink_msg_request_data_stream_pack(255, 0, &msg, 1, 0, streams[i], rates[i], 1);
    const uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    Serial1.write(buf, len);
  }
}

void handle_mavlink_rx() {
  mavlink_message_t msg; mavlink_status_t status;
  while (Serial1.available()) {
    const uint8_t c = Serial1.read();
    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
      switch (msg.msgid) {
        case MAVLINK_MSG_ID_GPS_RAW_INT: {
          mavlink_gps_raw_int_t gps; mavlink_msg_gps_raw_int_decode(&msg, &gps);
          pkt.lat = gps.lat / 1e7f; pkt.lon = gps.lon / 1e7f; pkt.alt = gps.alt / 1000.0f;
          break;
        }
        case MAVLINK_MSG_ID_ATTITUDE: {
          mavlink_attitude_t att; mavlink_msg_attitude_decode(&msg, &att);
          pkt.yaw = att.yaw * 180.0f / PI;
          break;
        }
        default: break;
      }
    }
  }
}

void send_rf24() { radio.write(&pkt, sizeof(pkt)); }

void setup() {
  Serial.begin(115200);
  Serial1.begin(MAV_BAUD, SERIAL_8N1, MAV_RX_PIN, MAV_TX_PIN);
  if (!radio.begin()) { Serial.println("NRF24 init failed"); while (true) delay(100);} 
  radio.setPALevel(RF24_PA_HIGH); radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(PIPE_ADDR); radio.stopListening();
  pkt = {0,0,0,0};
  Serial.println("TX(ESP32) ready");
}

void loop() {
  const unsigned long now = millis();
  if (now - lastTick >= intervalMs) { lastTick = now; send_heartbeat(); request_streams(); send_rf24(); }
  handle_mavlink_rx();
}
