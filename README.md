# ESP-MAVLink

ESP32 / ESP32-C3 SuperMini와 Pixhawk(ArduPilot/PX4) 간 MAVLink 통신을 빠르게 연결하기 위한 실전 가이드입니다. 핀맵에 맞춘 배선도, TELEM 포트 설정 가이드, 필수 라이브러리 설치 목록을 포함합니다.

## 무엇을 포함하나요
- 핀맵에 맞춘 배선도(ESP32-C3 SuperMini 중심, ESP32 예시 포함)
- Pixhawk TELEM 포트(DF13/JST-GH) 연결/설정 가이드
- Arduino IDE 보드/라이브러리 설치 체크리스트
- 빠른 점검(Heartbeat 송수신, GPS/Attitude 수신) 절차

---

## 빠른 배선(ESP32-C3 SuperMini ↔ Pixhawk TELEM)
- UART 속도(예시): 230400 bps (스케치에 맞춰 조정)
- 레벨: 3.3V TTL (5V 직결 금지)

| 연결 | ESP32-C3 SuperMini | Pixhawk TELEM | 비고 |
|---|---|---|---|
| MAV RX | GPIO20 | TX | Pixhawk TX → ESP RX (교차) |
| MAV TX | GPIO21 | RX | Pixhawk RX → ESP TX (교차) |
| GND | GND | GND | 반드시 공통 접지 |
| VCC(옵션) | 5V/3V3 | VCC | TELEM 포트 전류 여유 확인 후 사용 권장 |

> 코드 예시에서는 `SERIAL1(=HardwareSerial(1))`를 230400 bps로 사용하고, `MAV_RX_PIN=20`, `MAV_TX_PIN=21`을 가정합니다.

ESP32(클래식) 예시(테스트 스케치 기준):
- RX=16, TX=17로 `HardwareSerial(1)` 사용

---

## Pixhawk TELEM 포트 가이드
- 공통 핀 구성(DF13/JST-GH 계열):
  - VCC, TX, RX, CTS, RTS, GND (6핀)
- 기본 연결은 TX/RX/GND 3가지만으로 충분(하드웨어 흐름제어 미사용)
- 안전 수칙
  - 3.3V 레벨 준수: ESP/TELEM 모두 3.3V TTL
  - 전원 공급: TELEM의 5V를 보드 전원으로 쓰는 경우 전류 한도 확인(권장: 외부 안정 전원)

### 펌웨어 설정
- ArduPilot (예: TELEM1 → SERIAL1)
  - `SERIAL1_PROTOCOL = 2` (MAVLink2)
  - `SERIAL1_BAUD = 230400` (스케치에 맞춰 57600/115200/921600 등으로 조정 가능)
  - 스트림 속도: GCS/OSD 용도에 맞춰 SRx_* 파라미터 조정
- PX4
  - `MAV_0_CONFIG = TELEM1` (포트 할당)
  - `MAV_0_BAUD = 230400` (보드/스케치와 동일 속도)

---

## Arduino 환경 준비
### 보드 패키지
- Boards Manager: "esp32 by Espressif Systems" 설치 (ESP32-C3 SuperMini/ESP32 지원)

### 필수 라이브러리
- MAVLink: Library Manager에서 "MAVLink"(ArduPilot) 설치 또는 MAVLink Generator로 C 라이브러리 생성 후 포함
- RF24 (NRF24L01 사용 시): "RF24 by TMRh20"
- LoRa (SX127x 사용 시): "LoRa by Sandeep Mistry"
- SPI: ESP32 코어에 포함

> 예제 스케치 일부는 `#include "MAVLink.h"` 형태를 사용합니다. 설치한 MAVLink 버전에 따라 `#include <mavlink.h>` 또는 경로형 include(`mavlink/common/mavlink.h`)가 필요할 수 있습니다.

---

## 동작 점검(추천 절차)
1) 배선 확인: TX↔RX 교차, GND 공통, 전원 안정
2) Pixhawk 파라미터 설정(위 TELEM 가이드 참조)
3) 스케치 업로드 후 시리얼 모니터(115200) 확인
   - Heartbeat 송신/수신 로그
   - GPS(`MAVLINK_MSG_ID_GPS_RAW_INT`), Attitude(`MAVLINK_MSG_ID_ATTITUDE`) 디코드 출력
4) 데이터가 없으면:
   - Baud 일치 확인, 케이블/커넥터 점검, 펌웨어 스트림/프로토콜 확인

---

## 확장(옵션)
- NRF24L01, LoRa(RA-02) 전송: 테스트 스케치에서는 MAVLink로 받은 GPS/자세를 무선 모듈로 브릿징
- 레이저 거리계(SEN0366): 동일 보드에서 `Serial1`을 MAVLink/SEN0366로 스위칭하여 주기적 취득 예시 존재

> 모듈별 SPI/UART 핀은 보드에 따라 다릅니다. 예제 스케치의 핀 정의를 기준으로 시작하고, 보드 핀맵에 맞게 수정하세요.

---

## 참고(예제 스케치 출발점)
- ESP32-C3 추정 예시: `MAV_RX_PIN=20`, `MAV_TX_PIN=21`, `Serial1 230400bps`
- ESP32 예시: `RX=16`, `TX=17`, `HardwareSerial(1)`

이 레포는 문서 중심이며, 실제 통신 테스트에는 사용 중인 보드/펌웨어에 맞춘 파라미터 조정이 필수입니다.
