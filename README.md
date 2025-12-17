# ESP-MAVLink

ESP32 / ESP32-C3 SuperMini와 Pixhawk(ArduPilot / PX4)를 TELEM 포트로 연결해 MAVLink 비행 정보를 받아오고, 이를 nRF24L01으로 무선 전송/수신하는 실전형 가이드입니다.

한 문장으로 요약하면: “기체(Pixhawk)의 비행 텔레메트리를 ESP로 읽어 RF24로 쏘고, 다른 쪽 ESP(또는 아두이노)에서 받아서 활용한다.”

---

## 왜 이 레포가 필요하나요?
비행 중인 드론의 GPS, 고도, 자세(Yaw 등) 데이터를 가볍게 꺼내서 무선으로 넘기고 싶을 때가 있습니다. 일반적인 지상국 대신, 소형 nRF24L01 링크로 최소한의 텔레메트리만 빠르게 주고받는 구성이 목표입니다. 이 레포는 아래를 제공합니다.

- ESP32-C3 SuperMini/ESP32 기준의 TELEM 배선 예시와 안정 팁
- ArduPilot / PX4에서의 TELEM 포트 설정 방법
- Arduino IDE 보드/라이브러리 설치 체크리스트
- TX(송신) / RX(수신) 예제 스케치: MAVLink → RF24 전송, RF24 → 시리얼 출력

---

## 아키텍처와 데이터 흐름
1) Pixhawk TELEM 포트에서 MAVLink 메시지 송출
2) ESP(기체 탑재)가 UART로 MAVLink 수신 → `GPS_RAW_INT` / `ATTITUDE` 등에서 필요한 필드 추출
3) 추출 데이터를 경량 `TelemetryPacket(lat, lon, alt, yaw)`로 패킹
4) nRF24L01을 통해 데이터 송신
5) 지상 수신기(ESP 또는 Arduino Nano)가 RF24로 수신 → 시리얼/화면/UI로 활용

> 본 레포 예제는 Heartbeat 송신 + 데이터 스트림 요청(EXTENDED_STATUS, EXTRA1)을 통해 기체 측 데이터가 꾸준히 오도록 설정합니다.

---

## 하드웨어 구성
- 비행 컨트롤러: Pixhawk 계열(ArduPilot / PX4)
- 송신부(TX): ESP32-C3 SuperMini 또는 ESP32 + nRF24L01
- 수신부(RX): ESP32-C3 SuperMini / ESP32 / Arduino Nano + nRF24L01 (중 택1)
- 전원: 모두 3.3V 레벨. TELEM의 5V 라인은 전류 여유 확인 후 사용 권장(가급적 외부 안정 전원)

### TELEM 배선(핵심)
- 신호 레벨: 3.3V TTL (5V 직결 금지)
- 케이블: TX↔RX 교차, GND 공통
- 권장 Baud: 230400bps (상황에 따라 57600/115200/921600 등 조정 가능)

| 보드 | ESP RX | ESP TX | Pixhawk TELEM |
|---|---:|---:|---|
| ESP32-C3 SuperMini | GPIO20 | GPIO21 | TX↔RX 교차, GND 공통 |
| ESP32(클래식) | GPIO16 | GPIO17 | TX↔RX 교차, GND 공통 |

### nRF24L01 배선(예시)
- CE: 10, CSN: 9 (예제 기본값. 필요 시 코드에서 변경)
- 전원/그라운드: 3.3V 안정 공급(노이즈에 민감하므로 충분한 디커플링 권장)

---

## Pixhawk 설정 가이드
- ArduPilot(예: TELEM1 → SERIAL1)
  - `SERIAL1_PROTOCOL = 2` (MAVLink2)
  - `SERIAL1_BAUD = 230400` (보드와 일치)
  - 스트림 비율: 목적에 맞춰 SRx_* 파라미터 조정
- PX4
  - `MAV_0_CONFIG = TELEM1` (포트 할당)
  - `MAV_0_BAUD = 230400`

> 핵심은 양단 Baud/프로토콜 일치와 TX↔RX 교차, 그리고 GND 공통입니다.

---

## 개발 환경(Arduino IDE)
- 보드 패키지: Boards Manager에서 "esp32 by Espressif Systems" 설치
- 라이브러리
  - MAVLink: Library Manager(ArduPilot) 또는 Generator로 C 헤더 생성 후 포함
  - RF24: "RF24 by TMRh20"
  - LoRa(옵션): "LoRa by Sandeep Mistry" (본 레포의 기본 경로는 RF24)
  - SPI: ESP32 코어 포함

> 이 레포의 `include/mavlink_include.h`는 설치 형태에 따라 `mavlink/common/mavlink.h`, `mavlink.h`, `"MAVLink.h"` 중 적절히 포함하도록 되어 있습니다.

---

## 예제 폴더 안내
- 공용 헤더
  - `include/mavlink_include.h` — MAVLink 헤더 자동 포함 래퍼
  - `include/telemetry_packet.h` — RF 전송용 패킷 구조체
- 송신(TX)
  - `examples/tx/esp32c3-supermini` — ESP32-C3 SuperMini + RF24, TELEM UART1(GPIO20/21)
  - `examples/tx/esp32c3` — ESP32(클래식) + RF24, TELEM UART1(GPIO16/17)
  - (참고) Arduino Nano는 TX에서 MAVLink 용량 이슈로 제외
- 수신(RX)
  - `examples/rx/esp32c3-supermini`
  - `examples/rx/esp32c3`
  - `examples/rx/arduino-nano`

각 예제는 동일한 RF24 파이프 주소("24613"), 데이터레이트(`RF24_250KBPS`), 출력 파워(`PA_HIGH`)를 기본으로 사용합니다.

---

## 빌드 & 업로드 빠른 절차
1. Arduino IDE에서 보드/포트 선택(ESP32 계열)
2. 예제 스케치 열기(TX 1종, RX 1종)
3. 필요 시 핀/파이프 주소/보드 변수를 수정
4. 업로드 후 시리얼 모니터(115200)로 상태 확인
   - TX: Heartbeat 송신/스트림 요청, RF24 전송 로그
   - RX: 수신 패킷 `lat, lon, alt, yaw` 출력

---

## 트러블슈팅 체크리스트
- 통신이 전혀 안 온다
  - Baud/프로토콜(ArduPilot/PX4)과 보드 설정 일치 확인
  - TX↔RX 교차, GND 공통 재확인
  - 케이블/커넥터/핀맵 재점검(보드 변형마다 UART/SPI 핀 다를 수 있음)
- 데이터가 듬성듬성 온다 / 깨진다
  - 전원/그라운드 품질, 배선 길이/결속 확인
  - Baud를 낮춰 테스트(115200 등)
  - RF24 안테나/모듈 상태와 채널 간섭 확인
- Arduino Nano 용량 이슈
  - TX 측에서 MAVLink를 포함하면 용량 한계로 구동 실패 가능 → 본 레포는 Nano TX를 제공하지 않습니다. Nano는 RX 전용 예제를 제공합니다.

---

## 다음 단계(옵션)
- LoRa(RA-02) 전송 경로 추가
- 보드별 실제 사진 및 배선 다이어그램 첨부
- Heartbeat-only 최소 예제(무선 미포함) 제공

필요한 보드/모듈 조합이 있다면 이슈로 남겨주세요. 바로 추가해 보겠습니다.
