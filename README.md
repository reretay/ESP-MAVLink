# ESP-MAVLink

ESP32 / ESP32-C3 SuperMini와 Pixhawk(ArduPilot / PX4)를 TELEM 포트로 연결해 MAVLink 비행 텔레메트리를 수집하고, nRF24L01으로 무선 송수신하기 위한 구성과 예제를 제공한다.

---

## 목적
- Pixhawk → TELEM → ESP(UART) → MAVLink 파싱 → `TelemetryPacket(lat, lon, alt, yaw)` 생성 → nRF24L01 송신
- 수신 측(ESP 또는 Arduino Nano)에서 RF24로 패킷 수신 후 시리얼/응용 처리

---

## 데이터 흐름
1) Pixhawk가 TELEM 포트로 MAVLink 메시지를 출력한다.
2) ESP가 UART로 수신하여 `GPS_RAW_INT`, `ATTITUDE`를 파싱한다.
3) 파싱한 값을 `TelemetryPacket`으로 패킹한다.
4) nRF24L01로 패킷을 송신한다.
5) 수신 노드가 RF24로 패킷을 수신하고 시리얼로 출력하거나 애플리케이션에 전달한다.

예제는 Heartbeat 송신 및 데이터 스트림 요청(EXTENDED_STATUS, EXTRA1)을 포함한다.

---

## 하드웨어 및 배선
- 신호 레벨: 3.3V TTL
- TELEM 연결: TX↔RX 교차, GND 공통
- 권장 Baud: 230400bps (환경에 맞춰 57600/115200/921600 등 조정 가능)

### TELEM(UART) 핀맵
| 보드 | ESP RX | ESP TX | 비고 |
|---|---:|---:|---|
| ESP32-C3 SuperMini | GPIO20 | GPIO21 | Pixhawk TELEM TX→ESP RX, RX→TX |
| ESP32(클래식) | GPIO16 | GPIO17 | `HardwareSerial(1)` 사용 |

### nRF24L01 핀맵
- CE/CSN은 임의 GPIO 사용 가능(예제 기본값 사용). SCK/MISO/MOSI는 보드 SPI 기본값을 사용하거나 `SPI.begin(SCLK, MISO, MOSI)`로 재지정한다.

ESP32-C3 SuperMini (예제 기본값)
- CE: 10
- CSN: 9
- SCK/MISO/MOSI: 보드 기본 SPI 핀 사용(보드 핀맵 참조)

ESP32(클래식) 일반적 예시
- CE: 10
- CSN: 9
- SCK: 18, MISO: 19, MOSI: 23 (VSPI 일반적 배치)

Arduino Nano(ATmega328P)
- CE: D9
- CSN: D10
- SCK: D13, MISO: D12, MOSI: D11 (하드웨어 SPI 고정)

전원: nRF24L01은 3.3V 안정 전원을 권장(디커플링 캐패시터 권장).

---

## Pixhawk 설정
ArduPilot(예: TELEM1)
- `SERIAL1_PROTOCOL = 2` (MAVLink2)
- `SERIAL1_BAUD = 230400`

PX4
- `MAV_0_CONFIG = TELEM1`
- `MAV_0_BAUD = 230400`

핵심: 양단 Baud/프로토콜 일치, TX↔RX 교차, GND 공통.

---

## 개발 환경
- 보드 패키지: "esp32 by Espressif Systems"
- 라이브러리
  - MAVLink (ArduPilot Library Manager 또는 Generator 생성 헤더)
  - RF24 by TMRh20
  - SPI (ESP32 코어 포함)

`include/mavlink_include.h`는 설치 형태에 맞춰 적절한 MAVLink 헤더를 포함한다.

---

## 예제 구조
- 공용
  - `include/mavlink_include.h` — MAVLink 헤더 포함 래퍼
  - `include/telemetry_packet.h` — RF 전송용 패킷 구조체
- TX
  - `examples/tx/esp32c3-supermini` — UART1(GPIO20/21), RF24 송신
  - `examples/tx/esp32c3` — UART1(GPIO16/17), RF24 송신
  - Arduino Nano TX는 제공하지 않음(용량 이슈)
- RX
  - `examples/rx/esp32c3-supermini`
  - `examples/rx/esp32c3`
  - `examples/rx/arduino-nano`

RF24 기본값: 파이프 주소 "24613", 데이터레이트 `RF24_250KBPS`, 출력 `PA_HIGH`.

---

## 빌드 절차
1) 보드/포트 선택
2) TX 1종, RX 1종 예제 열기
3) 필요 시 핀/주소/속도 수정
4) 업로드 후 시리얼(115200) 확인: TX(송출 로그), RX(패킷 출력)

---

## 점검 항목
- 통신 없음: Baud/프로토콜/배선 확인, 케이블/커넥터 점검
- 간헐/깨짐: 전원 품질, 배선 길이, RF 간섭 확인, Baud 하향 테스트
- Nano 용량: TX에서 MAVLink 포함 시 용량 초과 가능 → Nano는 RX 전용 사용
